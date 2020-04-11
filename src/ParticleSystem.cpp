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

    const int width = mNoiseSize;
    const int height = mNoiseSize;
    const int depth = mNoiseSize;

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

    mNoiseTex = ci::gl::Texture3d::create(mNoiseSize, mNoiseSize, mNoiseSize, tex3dFmt);
    mNoiseTex->update(data.data(), GL_RGBA, tex3dFmt.getDataType(), 0, mNoiseTex->getWidth(),
                      mNoiseTex->getHeight(), mNoiseTex->getDepth());
}

void ParticleSystem::setupBuffers() {
    mPos = ci::gl::Ssbo::create(sizeof(ci::vec4) * NUM_PARTICLES, nullptr, GL_STATIC_DRAW);
    mVel = ci::gl::Ssbo::create(sizeof(ci::vec4) * NUM_PARTICLES, nullptr, GL_STATIC_DRAW);

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

    mIndicesVbo = ci::gl::Vbo::create<uint32_t>(GL_ELEMENT_ARRAY_BUFFER, indices, GL_STATIC_DRAW);
}

void ParticleSystem::reset(float size) {
    auto *pos = reinterpret_cast<ci::vec4 *>( mPos->map(GL_WRITE_ONLY));
    for (size_t i = 0; i < NUM_PARTICLES; ++i) {
        pos[i] = ci::vec4(sfrand() * size, sfrand() * size, sfrand() * size, 1.0f);
    }
    mPos->unmap();

    auto *vel = reinterpret_cast<ci::vec4 *>( mVel->map(GL_WRITE_ONLY));
    for (size_t i = 0; i < NUM_PARTICLES; ++i) {
        vel[i] = ci::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    mVel->unmap();

}


// Unproject a coordinate back to to camera
ci::vec3 unproject(const ci::vec3 &point, const ci::CameraPersp &cam, const ci::ivec2 &window_size) {
    // Find the inverse Modelview-Projection-Matrix
    ci::mat4 mInvMVP = glm::inverse(cam.getProjectionMatrix() * cam.getViewMatrix());

    // Transform to normalized coordinates in the range [-1, 1]
    ci::vec4 pointNormal;
//    pointNormal.x = (point.x - mViewport.getX1()) / mViewport.getWidth() * 2.0f - 1.0f;
//    pointNormal.y = (point.y - mViewport.getY1()) / mViewport.getHeight() * 2.0f;
    pointNormal.x = (point.x) / window_size.x * 2.0f - 1.0f;
    pointNormal.y = (point.y) / window_size.y * 2.0f;
    pointNormal.z = 2.0f * point.z - 1.0f;
    pointNormal.w = 1.0f;

    // Find the object's coordinates
    ci::vec4 pointCoord = mInvMVP * pointNormal;
    if (pointCoord.w != 0.0f) {
        pointCoord.w = 1.0f / pointCoord.w;
    }


    // Return coordinate
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
    mParticleParams.numParticles = NUM_PARTICLES;
    // TODO
    auto world_coordinate = screenToWorld(mouse_position, cam, window_size);
    mParticleParams.attractor = ci::vec4(world_coordinate, 0.);
    mParticleParams.attractor.w = 0.0001f;


    // Invoke the compute shader to integrate the particles
    ci::gl::ScopedGlslProg prog(mUpdateProg);

    mParticleUpdateUbo->bufferSubData(0, sizeof(mParticleParams), &mParticleParams);
    ci::gl::ScopedTextureBind scoped3dTex(mNoiseTex);


    ci::gl::bindBufferBase(mPos->getTarget(), 1, mPos);
    ci::gl::bindBufferBase(mPos->getTarget(), 2, mVel);

    ci::gl::dispatchCompute(NUM_PARTICLES / WORK_GROUP_SIZE, 1, 1);
    // We need to block here on compute completion to ensure that the
    // computation is done before we render
    ci::gl::memoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ParticleSystem::setupShaders() {
    mUpdateProg = ci::gl::GlslProg::create(ci::gl::GlslProg::Format().compute(ci::app::loadAsset("particles.comp")));
    // Particle update ubo.
    mParticleUpdateUbo = ci::gl::Ubo::create(sizeof(mParticleParams), &mParticleParams, GL_DYNAMIC_DRAW);
    mParticleUpdateUbo->bindBufferBase(0);
    mUpdateProg->uniformBlock("ParticleParams", 0);
    mUpdateProg->uniform("noiseTex3D", 0);
}

void ParticleSystem::draw() {
    ci::gl::bindBufferBase(mPos->getTarget(), 1, mPos);
    ci::gl::ScopedBuffer scopedIndicex(mIndicesVbo);
    ci::gl::drawElements(GL_TRIANGLES, NUM_PARTICLES * 6, GL_UNSIGNED_INT, nullptr);
}

ParticleSystem::ParticleSystem() : mNoiseSize(16),
                                   mParticleParams(mNoiseSize) {
    setupNoiseTexture3D();
    setupBuffers();
    setupShaders();
}







