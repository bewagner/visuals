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
                : damping(0.95f),
                  noiseFreq(10.0f),
                  noiseStrength(0.001f),
                  invNoiseSize(1.0f / noiseSize) {
        }
        float numParticles{};
        float damping;
        float noiseFreq;
        float noiseStrength;
        float invNoiseSize;
    };

    Parameters parameters;


    explicit ParticleSystem();

    void reset(float size);

    void update(const ci::ivec2 &mouse_position, const ci::CameraPersp &cam, const ci::ivec2 &window_size);

    void draw() const;

private:

    const int noise_size_;
    ci::gl::SsboRef position_ssbo_;
    ci::gl::SsboRef velocity_ssbo_;
    ci::gl::Texture3dRef noise_texture_;
    ci::gl::VboRef indices_vbo_;
    ci::gl::UboRef particle_update_ubo_;
    ci::gl::GlslProgRef update_program_;

    static const int max_number_of_eye_pairs_ = 5;
    std::array<ci::vec4, max_number_of_eye_pairs_> eye_positions_;
    ci::gl::UboRef eye_positions_ubo_;

    void setupNoiseTexture3D();

    void setupShaders();

    void setupBuffers();


};


#endif //VISUALS_PARTICLESYSTEM_H
