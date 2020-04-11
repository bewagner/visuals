//
// Created by benjamin on 11.04.20.
//

#ifndef VISUALS_PARTICLESYSTEM_H
#define VISUALS_PARTICLESYSTEM_H

#include "cinder/gl/gl.h"
#include "cinder/gl/Ssbo.h"


struct ParticleParams {
    explicit ParticleParams(float noiseSize)
            : attractor(0.0f, 0.0f, 0.0f, 0.0f),
              damping(0.95f),
              noiseFreq(10.0f),
              noiseStrength(0.001f),
              invNoiseSize(1.0f / noiseSize) {
    }

    ci::vec4 attractor;
    float numParticles{};
    float damping;
    float noiseFreq;
    float noiseStrength;
    float invNoiseSize;
};


class ParticleSystem {
public:
    int mNoiseSize;
    ParticleParams mParticleParams;
    ci::gl::SsboRef mPos;
    ci::gl::SsboRef mVel;
    ci::gl::Texture3dRef mNoiseTex;
    ci::gl::VboRef mIndicesVbo;
    ci::gl::UboRef mParticleUpdateUbo;
    ci::gl::GlslProgRef mUpdateProg;

    explicit ParticleSystem();

    void reset(float size);

    void update(const ci::ivec2& mouse_position, const ci::CameraPersp& cam, const ci::ivec2& window_size);

    void draw();

private:
    void setupNoiseTexture3D();

    void setupShaders();

    void setupBuffers();


};


#endif //VISUALS_PARTICLESYSTEM_H
