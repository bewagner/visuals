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

    const int width = noise_size;
    const int height = noise_size;
    const int depth = noise_size;

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

    noise_texture = ci::gl::Texture3d::create(noise_size, noise_size, noise_size, tex3dFmt);
    noise_texture->update(data.data(), GL_RGBA, tex3dFmt.getDataType(), 0, noise_texture->getWidth(),
                          noise_texture->getHeight(), noise_texture->getDepth());
}

void ParticleSystem::setupBuffers() {
    position_ssbo = ci::gl::Ssbo::create(sizeof(ci::vec4) * NUM_PARTICLES, nullptr, GL_STATIC_DRAW);
    velocity_ssbo = ci::gl::Ssbo::create(sizeof(ci::vec4) * NUM_PARTICLES, nullptr, GL_STATIC_DRAW);

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

    indices_vbo = ci::gl::Vbo::create<uint32_t>(GL_ELEMENT_ARRAY_BUFFER, indices, GL_STATIC_DRAW);
}

void ParticleSystem::reset(float size) {
    auto *pos = reinterpret_cast<ci::vec4 *>( position_ssbo->map(GL_WRITE_ONLY));
    for (size_t i = 0; i < NUM_PARTICLES; ++i) {
        pos[i] = ci::vec4(sfrand() * size, sfrand() * size, sfrand() * size, 1.0f);
    }
    position_ssbo->unmap();

    auto *vel = reinterpret_cast<ci::vec4 *>( velocity_ssbo->map(GL_WRITE_ONLY));
    for (size_t i = 0; i < NUM_PARTICLES; ++i) {
        vel[i] = ci::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    velocity_ssbo->unmap();
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
    // TODO
    auto world_coordinate = screenToWorld(mouse_position, cam, window_size);
    parameters.attractor = ci::vec4(world_coordinate, 0.);
    parameters.attractor.w = 0.0001f;


    // Invoke the compute shader to integrate the particles
    ci::gl::ScopedGlslProg prog(update_program);

    particle_update_ubo->bufferSubData(0, sizeof(parameters), &parameters);
    ci::gl::ScopedTextureBind scoped3dTex(noise_texture);


    ci::gl::bindBufferBase(position_ssbo->getTarget(), 1, position_ssbo);
    ci::gl::bindBufferBase(position_ssbo->getTarget(), 2, velocity_ssbo);

    ci::gl::dispatchCompute(NUM_PARTICLES / WORK_GROUP_SIZE, 1, 1);
    // We need to block here on compute completion to ensure that the
    // computation is done before we render
    ci::gl::memoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ParticleSystem::setupShaders() {
    update_program = ci::gl::GlslProg::create(ci::gl::GlslProg::Format().compute(ci::app::loadAsset("particles.comp")));
    // Particle update ubo.
    particle_update_ubo = ci::gl::Ubo::create(sizeof(parameters), &parameters, GL_DYNAMIC_DRAW);
    particle_update_ubo->bindBufferBase(0);
    update_program->uniformBlock("ParticleParams", 0);
    update_program->uniform("noiseTex3D", 0);
}

void ParticleSystem::draw() const {
    ci::gl::context()->setDefaultShaderVars();

    ci::gl::enableAdditiveBlending();

    ci::gl::disable(GL_DEPTH_TEST);
    ci::gl::disable(GL_CULL_FACE);
    ci::gl::bindBufferBase(position_ssbo->getTarget(), 1, position_ssbo);
    ci::gl::ScopedBuffer scopedIndicex(indices_vbo);
    ci::gl::drawElements(GL_TRIANGLES, NUM_PARTICLES * 6, GL_UNSIGNED_INT, nullptr);

    ci::gl::disableAlphaBlending();
}

ParticleSystem::ParticleSystem() : noise_size(16),
                                   parameters(static_cast<float>(noise_size)) {
    setupNoiseTexture3D();
    setupBuffers();
    setupShaders();
}







