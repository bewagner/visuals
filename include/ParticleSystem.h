//
// Created by benjamin on 11.04.20.
//

#ifndef VISUALS_PARTICLESYSTEM_H
#define VISUALS_PARTICLESYSTEM_H

#include "cinder/gl/gl.h"


class ParticleSystem {
public:
    struct Parameters {
        explicit Parameters(float noiseSize)
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


    int noise_size;
    Parameters parameters;
    ci::gl::SsboRef position_ssbo;
    ci::gl::SsboRef velocity_ssbo;
    ci::gl::Texture3dRef noise_texture;
    ci::gl::VboRef indices_vbo;
    ci::gl::UboRef particle_update_ubo;
    ci::gl::GlslProgRef update_program;

    explicit ParticleSystem();

    void reset(float size);

    void update(const ci::ivec2 &mouse_position, const ci::CameraPersp &cam, const ci::ivec2 &window_size);

    void draw();

private:
    void setupNoiseTexture3D();

    void setupShaders();

    void setupBuffers();


};


#endif //VISUALS_PARTICLESYSTEM_H
