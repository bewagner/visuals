//
// Created by benjamin on 11.04.20.
//
#include "ParticleSystem.h"
#include "cinder/Rand.h"
#include "cinder/app/App.h"


// Note: When changing anything below, also change the values in the compute shader
const int ParticleSystem::NUM_PARTICLES = 1u << 18u;
const int ParticleSystem::WORK_GROUP_SIZE = 128;
const size_t ParticleSystem::MAX_NUMBER_OF_EYE_PAIRS = 10;

struct BufferIndices {
    // Note: When changing anything here, also change the values in the compute shader
    static const int PARTICLE_UPDATE_UBO = 0;
    static const int POSITION_SSBO = 1;
    static const int VELOCITY_SSBO = 2;
    static const int EYE_POSITION_UBO = 3;
};


/// Get a random float in the range [-1, 1]
/// \return Random float in the range [-1, 1]
float sfrand() {
    return ci::randPosNegFloat(-1.0f, 1.0f);
}

void ParticleSystem::updateNoiseTexture3D() {
    ci::gl::Texture3d::Format tex3dFmt;
    tex3dFmt.setWrapR(GL_REPEAT);
    tex3dFmt.setWrapS(GL_REPEAT);
    tex3dFmt.setWrapT(GL_REPEAT);
    tex3dFmt.setMagFilter(GL_LINEAR);
    tex3dFmt.setMinFilter(GL_LINEAR);
    tex3dFmt.setDataType(GL_FLOAT);
    tex3dFmt.setInternalFormat(GL_RGBA8_SNORM);

    const int width = noise_size;
    const int height = noise_size;
    const int depth = noise_size;

    std::vector<float> data(static_cast<size_t>(width * height * depth * 4));
    unsigned int i = 0;
    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                data[i++] = sfrand();
                data[i++] = sfrand();
                data[i++] = sfrand();
                data[i++] = sfrand();
            }
        }
    }

    noise_texture_ = ci::gl::Texture3d::create(noise_size, noise_size, noise_size, tex3dFmt);
    noise_texture_->update(data.data(), GL_RGBA, static_cast<GLenum>(tex3dFmt.getDataType()), 0, noise_texture_->getWidth(),
                           noise_texture_->getHeight(), noise_texture_->getDepth());
}

void ParticleSystem::setupBuffers() {
    position_ssbo_ = ci::gl::Ssbo::create(sizeof(ci::vec4) * NUM_PARTICLES, nullptr, GL_STATIC_DRAW);
    velocity_ssbo_ = ci::gl::Ssbo::create(sizeof(ci::vec4) * NUM_PARTICLES, nullptr, GL_STATIC_DRAW);

    std::vector<uint32_t> indices(NUM_PARTICLES * 6);
    // the index buffer is a classic "two-tri quad" array.
    // This may seem odd, given that the compute buffer contains a single
    // vector for each particle.  However, the shader takes care of this
    // by indexing the compute shader buffer with a /4.  The value mod 4
    // is used to compute the offset from the vertex site, and each of the
    // four indices in a given quad references the same center point
    for (unsigned int i = 0, j = 0; i < NUM_PARTICLES; ++i) {
        uint32_t index = i << 2u;
        indices[j++] = index;
        indices[j++] = index + 1;
        indices[j++] = index + 2;
        indices[j++] = index;
        indices[j++] = index + 2;
        indices[j++] = index + 3;
    }
    indices_vbo_ = ci::gl::Vbo::create<uint32_t>(GL_ELEMENT_ARRAY_BUFFER, indices, GL_STATIC_DRAW);

    particle_update_ubo_ = ci::gl::Ubo::create(sizeof(parameters), &parameters, GL_DYNAMIC_DRAW);
    particle_update_ubo_->bindBufferBase(BufferIndices::PARTICLE_UPDATE_UBO);

    eye_positions_ubo_ = ci::gl::Ubo::create(sizeof(ci::vec4) * MAX_NUMBER_OF_EYE_PAIRS, eye_positions_.data(),
                                             GL_DYNAMIC_DRAW);
    eye_positions_ubo_->bindBufferBase(BufferIndices::EYE_POSITION_UBO);
}

void ParticleSystem::reset(float size) {
    auto *pos = reinterpret_cast<ci::vec4 *>( position_ssbo_->map(GL_WRITE_ONLY));
    for (size_t i = 0; i < NUM_PARTICLES; ++i) {
        pos[i] = ci::vec4(sfrand() * size, sfrand() * size, sfrand() * size, 1.0f);
    }
    position_ssbo_->unmap();

    auto *vel = reinterpret_cast<ci::vec4 *>( velocity_ssbo_->map(GL_WRITE_ONLY));
    for (size_t i = 0; i < NUM_PARTICLES; ++i) {
        vel[i] = ci::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    velocity_ssbo_->unmap();
}


/// Unproject a point back to the camera view
/// \param point Point to unproject
/// \param camera Camera to use
/// \param window_size Current app window size
/// \return Unprojected point
ci::vec3 unproject(const ci::vec3 &point, const ci::CameraPersp &camera, const ci::ivec2 &window_size) {
    // Find the inverse Modelview-Projection-Matrix
    ci::mat4 mInvMVP = glm::inverse(camera.getProjectionMatrix() * camera.getViewMatrix());

    // Transform to normalized coordinates in the range [-1, 1]
    ci::vec4 pointNormal;
    auto window_x = static_cast<float>(window_size.x);
    auto window_y = static_cast<float>(window_size.y);
    pointNormal.x = (point.x) / window_x * 2.0f - 1.0f;
    pointNormal.y = (point.y) / window_y * 2.0f;
    pointNormal.z = 2.0f * point.z - 1.0f;
    pointNormal.w = 1.0f;

    // Find the object's coordinates
    ci::vec4 pointCoord = mInvMVP * pointNormal;
    if (pointCoord.w != 0.0f) {
        pointCoord.w = 1.0f / pointCoord.w;
    }

    return ci::vec3(
            pointCoord.x * pointCoord.w,
            pointCoord.y * pointCoord.w,
            pointCoord.z * pointCoord.w
    );
}

/// Transform a point from screen to world coordinates
/// \param point Point to transform
/// \param camera Camera to use
/// \param window_size Current app window size
/// \return Transformed point
ci::vec3 screenToWorld(const ci::ivec2 &point, const ci::CameraPersp &camera, const ci::ivec2 &window_size) {
    // Find near and far plane intersections
    ci::vec3 point3f = ci::vec3(static_cast<float>(point.x),
                                static_cast<float>(window_size.y) * 0.5f - static_cast<float>( point.y),
                                0.0f);
    ci::vec3 nearPlane = unproject(point3f, camera, window_size);
    ci::vec3 farPlane = unproject(ci::vec3(point3f.x, point3f.y, 1.0f), camera, window_size);

    // Calculate X, Y and return point
    float theta = (0.0f - nearPlane.z) / (farPlane.z - nearPlane.z);
    return ci::vec3(
            nearPlane.x + theta * (farPlane.x - nearPlane.x),
            nearPlane.y + theta * (farPlane.y - nearPlane.y),
            0.0f
    );
}


void
ParticleSystem::update(const AppState &app_state) {
    parameters.numParticles = NUM_PARTICLES;

    // Invoke the compute shader to integrate the particles
    ci::gl::ScopedGlslProg prog(update_program_);



    // TODO Delete eye_positions variable
    eye_positions_ = app_state.eye_pairs;
    if (app_state.attract_to_mouse) {
        eye_positions_.emplace_back(ci::vec4(app_state.mouse_position, 0., 0.));
    }

    auto *pair_ubo = (ci::vec4 *) eye_positions_ubo_->mapWriteOnly();
    for (unsigned int i = 0; i < std::min(MAX_NUMBER_OF_EYE_PAIRS, eye_positions_.size()); ++i) {
        auto world_coordinate = ci::vec4(screenToWorld(eye_positions_[i], app_state.camera, app_state.window_size),
                                         0.0007f);
        *pair_ubo = world_coordinate;
        pair_ubo++;
    }
    eye_positions_ubo_->unmap();
    update_program_->uniform("numberOfEyePairs", static_cast<unsigned int>(eye_positions_.size()));


    particle_update_ubo_->bufferSubData(0, sizeof(parameters), &parameters);
    ci::gl::ScopedTextureBind scoped3dTex(noise_texture_);


    ci::gl::bindBufferBase(position_ssbo_->getTarget(), BufferIndices::POSITION_SSBO, position_ssbo_);
    ci::gl::bindBufferBase(velocity_ssbo_->getTarget(), BufferIndices::VELOCITY_SSBO, velocity_ssbo_);

    ci::gl::dispatchCompute(NUM_PARTICLES / WORK_GROUP_SIZE, 1, 1);
    // We need to block here on compute completion to ensure that the
    // computation is done before we render
    ci::gl::memoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ParticleSystem::setupShaders() {
    update_program_ = ci::gl::GlslProg::create(
            ci::gl::GlslProg::Format().compute(ci::app::loadAsset("particles.comp")));
    // Particle update ubo.

    update_program_->uniformBlock("ParticleParams", BufferIndices::PARTICLE_UPDATE_UBO);
    update_program_->uniformBlock("EyePositions", BufferIndices::EYE_POSITION_UBO);
    update_program_->uniform("noiseTex3D", 0);
}

void ParticleSystem::draw() const {
    ci::gl::context()->setDefaultShaderVars();

    ci::gl::enableAdditiveBlending();

    ci::gl::disable(GL_DEPTH_TEST);
    ci::gl::disable(GL_CULL_FACE);
    ci::gl::bindBufferBase(position_ssbo_->getTarget(), BufferIndices::POSITION_SSBO, position_ssbo_);
    ci::gl::ScopedBuffer scopedIndicex(indices_vbo_);
    ci::gl::drawElements(GL_TRIANGLES, NUM_PARTICLES * 6, GL_UNSIGNED_INT, nullptr);

    ci::gl::disableAlphaBlending();
}

ParticleSystem::ParticleSystem() : noise_size(16),
                                   parameters(static_cast<float>(noise_size)) {
    updateNoiseTexture3D();
    setupBuffers();
    setupShaders();
}







