//
// Created by benjamin on 11.04.20.
//

#include "ParticleSystem.h"
#include "cinder/Rand.h"
#include "cinder/app/App.h"

const int NUM_PARTICLES = 1u << 18u;
const int WORK_GROUP_SIZE = 128;

float sfrand() {
    return ci::randPosNegFloat(-1.0f, 1.0f);
}

void ParticleSystem::setupNoiseTexture3D() {
    ci::gl::Texture3d::Format tex3dFmt;
    tex3dFmt.setWrapR(GL_REPEAT);
    tex3dFmt.setWrapS(GL_REPEAT);
    tex3dFmt.setWrapT(GL_REPEAT);
    tex3dFmt.setMagFilter(GL_LINEAR);
    tex3dFmt.setMinFilter(GL_LINEAR);
    tex3dFmt.setDataType(GL_FLOAT);
    tex3dFmt.setInternalFormat(GL_RGBA8_SNORM);

    const int width = noise_size_;
    const int height = noise_size_;
    const int depth = noise_size_;

    std::vector<float> data(width * height * depth * 4);
    int i = 0;
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

    noise_texture_ = ci::gl::Texture3d::create(noise_size_, noise_size_, noise_size_, tex3dFmt);
    noise_texture_->update(data.data(), GL_RGBA, tex3dFmt.getDataType(), 0, noise_texture_->getWidth(),
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
    for (size_t i = 0, j = 0; i < NUM_PARTICLES; ++i) {
        size_t index = i << 2u;
        indices[j++] = index;
        indices[j++] = index + 1;
        indices[j++] = index + 2;
        indices[j++] = index;
        indices[j++] = index + 2;
        indices[j++] = index + 3;
    }
    indices_vbo_ = ci::gl::Vbo::create<uint32_t>(GL_ELEMENT_ARRAY_BUFFER, indices, GL_STATIC_DRAW);

    particle_update_ubo_ = ci::gl::Ubo::create(sizeof(parameters), &parameters, GL_DYNAMIC_DRAW);
    particle_update_ubo_->bindBufferBase(0);

    eye_positions_ubo_ = ci::gl::Ubo::create(sizeof(ci::vec4) * max_number_of_eye_pairs_, eye_positions_.data(),
                                             GL_DYNAMIC_DRAW);
    eye_positions_ubo_->bindBufferBase(3);
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


// Unproject a coordinate back to to camera
ci::vec3 unproject(const ci::vec3 &point, const ci::CameraPersp &cam, const ci::ivec2 &window_size) {
    // Find the inverse Modelview-Projection-Matrix
    ci::mat4 mInvMVP = glm::inverse(cam.getProjectionMatrix() * cam.getViewMatrix());

    // Transform to normalized coordinates in the range [-1, 1]
    ci::vec4 pointNormal;
    pointNormal.x = (point.x) / window_size.x * 2.0f - 1.0f;
    pointNormal.y = (point.y) / window_size.y * 2.0f;
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

ci::vec3 screenToWorld(const ci::ivec2 &point, const ci::CameraPersp &cam, const ci::ivec2 &window_size) {
    // Find near and far plane intersections
    ci::vec3 point3f = ci::vec3((float) point.x, window_size.y * 0.5f - (float) point.y, 0.0f);
    ci::vec3 nearPlane = unproject(point3f, cam, window_size);
    ci::vec3 farPlane = unproject(ci::vec3(point3f.x, point3f.y, 1.0f), cam, window_size);

    // Calculate X, Y and return point
    float theta = (0.0f - nearPlane.z) / (farPlane.z - nearPlane.z);
    return ci::vec3(
            nearPlane.x + theta * (farPlane.x - nearPlane.x),
            nearPlane.y + theta * (farPlane.y - nearPlane.y),
            0.0f
    );
}


void ParticleSystem::update(const ci::ivec2 &mouse_position, const ci::CameraPersp &cam, const ci::ivec2 &window_size) {
    parameters.numParticles = NUM_PARTICLES;

    // Invoke the compute shader to integrate the particles
    ci::gl::ScopedGlslProg prog(update_program_);

    // TODO
    auto world_coordinate = ci::vec4(screenToWorld(mouse_position, cam, window_size), 0.0002f);

    auto *pair_ubo = (ci::vec4 *) eye_positions_ubo_->mapWriteOnly();
    for (int i = 0; i < max_number_of_eye_pairs_; ++i) {
        auto current_coordinate = world_coordinate;
        current_coordinate.x += i * 0.1;
        current_coordinate.y += i * 0.1;
        *pair_ubo = current_coordinate;
        pair_ubo++;
    }
    eye_positions_ubo_->unmap();


    particle_update_ubo_->bufferSubData(0, sizeof(parameters), &parameters);
    ci::gl::ScopedTextureBind scoped3dTex(noise_texture_);


    ci::gl::bindBufferBase(position_ssbo_->getTarget(), 1, position_ssbo_);
    ci::gl::bindBufferBase(position_ssbo_->getTarget(), 2, velocity_ssbo_);

    ci::gl::dispatchCompute(NUM_PARTICLES / WORK_GROUP_SIZE, 1, 1);
    // We need to block here on compute completion to ensure that the
    // computation is done before we render
    ci::gl::memoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ParticleSystem::setupShaders() {
    update_program_ = ci::gl::GlslProg::create(
            ci::gl::GlslProg::Format().compute(ci::app::loadAsset("particles.comp")));
    // Particle update ubo.

    update_program_->uniformBlock("ParticleParams", 0);
    update_program_->uniformBlock("EyePositions", 3);
    update_program_->uniform("noiseTex3D", 0);
}

void ParticleSystem::draw() const {
    ci::gl::context()->setDefaultShaderVars();

    ci::gl::enableAdditiveBlending();

    ci::gl::disable(GL_DEPTH_TEST);
    ci::gl::disable(GL_CULL_FACE);
    ci::gl::bindBufferBase(position_ssbo_->getTarget(), 1, position_ssbo_);
    ci::gl::ScopedBuffer scopedIndicex(indices_vbo_);
    ci::gl::drawElements(GL_TRIANGLES, NUM_PARTICLES * 6, GL_UNSIGNED_INT, nullptr);

    ci::gl::disableAlphaBlending();
}

ParticleSystem::ParticleSystem() :
        parameters(16.), noise_size_(16) {
    setupNoiseTexture3D();
    setupBuffers();
    setupShaders();
}







