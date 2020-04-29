//
// Created by benjamin on 11.04.20.
//

#ifndef VISUALS_PARTICLESYSTEM_H
#define VISUALS_PARTICLESYSTEM_H

#include "cinder/gl/gl.h"

class ParticleSystem {
public:
    int noise_size;

    struct Parameters {
        explicit Parameters(float noise_size)
                : damping(0.95f),
                  noiseFreq(10.0f),
                  noiseStrength(0.001f),
                  invNoiseSize(1.0f / noise_size) {
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

    struct AppState {
        const ci::ivec2 &mouse_position;
        const ci::CameraPersp &camera;
        const ci::ivec2 &window_size;
        const bool attract_to_mouse;
        const std::vector<ci::vec4> & eye_pairs;
    };

    void update(const AppState& app_state);

    void draw() const;

    void updateNoiseTexture3D();




private:

    ci::gl::SsboRef position_ssbo_;
    ci::gl::SsboRef velocity_ssbo_;
    ci::gl::Texture3dRef noise_texture_;
    ci::gl::VboRef indices_vbo_;
    ci::gl::UboRef particle_update_ubo_;
    ci::gl::GlslProgRef update_program_;

    static const size_t MAX_NUMBER_OF_EYE_PAIRS;
    static const int NUM_PARTICLES;
    static const int WORK_GROUP_SIZE;

    std::vector<ci::vec4> eye_positions_;
    ci::gl::UboRef eye_positions_ubo_;

    void setupShaders();

    void setupBuffers();
};


#endif //VISUALS_PARTICLESYSTEM_H
