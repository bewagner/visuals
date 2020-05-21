//----------------------------------------------------------------------------------
// File:        ComputeParticles/ParticleSystem.cpp
// Url:         https://github.com/NVIDIAGameWorks/OpenGLSamples/tree/master/samples/es3aep-kepler/ComputeParticles
// SDK Version: v1.2 
// Email:       gameworks@nvidia.com
// Site:        http://developer.nvidia.com/
//
// Copyright (c) 2014, NVIDIA CORPORATION. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------------

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "cinder/Rand.h"
#include "cinder/params/Params.h"
#include "detector/Detector.h"
#include "CameraHandler.h"
#include "ParticleSystem.h"

using namespace ci;
using namespace ci::app;


//! This sample was ported from https://github.com/NVIDIAGameWorks/OpenGLSamples/tree/master/samples/es3aep-kepler/ComputeParticles
class NVidiaComputeParticlesApp : public App {
public:
    NVidiaComputeParticlesApp();

    void update() override;

    void draw() override;

    void setupShaders();

    void keyDown(KeyEvent event) override;

    ParticleSystem particle_system_;
    gl::GlslProgRef mRenderProg;

    params::InterfaceGlRef mParams;
    CameraPersp mCam;
    CameraUi mCamUi;

    bool mReset;
    bool mAttractToMouse;
    float mTime;
    float mPrevElapsedSeconds;

    Detector detector;
    CameraHandler cameraHandler;
};

NVidiaComputeParticlesApp::NVidiaComputeParticlesApp()
        : mCam(getWindowWidth(), getWindowHeight(), 45.0f, 0.1f, 10.0f),
          mReset(false),
          mAttractToMouse(false),
          mTime(0.0f),
          mPrevElapsedSeconds(0.0f) {

    mReset = false;
    mTime = 0.0f;
    mPrevElapsedSeconds = 0.0f;

    setupShaders();
    particle_system_.reset(0.5f);
    CI_CHECK_GL();

    mCam.lookAt(vec3(0.0f, 0.0f, -3.0f), vec3(0));

    mParams = params::InterfaceGl::create("Settings", toPixels(ivec2(225, 180)));
    mParams->addSeparator();
    mParams->addParam("Noise strength", &(particle_system_.parameters.noiseStrength)).min(0.0f).max(0.01f).step(
            0.001f);
    mParams->addParam("Noise frequency", &(particle_system_.parameters.noiseFreq)).min(0.0f).max(20.0f).step(1.0f);

    mParams->addParam("Damping", &(particle_system_.parameters.damping)).min(0.1f).max(0.99f).step(0.01f);
    mParams->addParam("Noise size", &(particle_system_.noise_size)).min(3).max(25).step(1).updateFn(
            [&]() {
                particle_system_.updateNoiseTexture3D();
                particle_system_.parameters.invNoiseSize = 1.f / static_cast<float>(particle_system_.noise_size);
            }
    );
    mParams->addSeparator();
    mParams->addParam("Attract to mouse", &mAttractToMouse);
    mParams->addParam("Reset", &mReset);

    mCamUi = CameraUi(&mCam, getWindow());
}

void NVidiaComputeParticlesApp::setupShaders() {
    try {
        mRenderProg = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("render.vert"))
                                                   .fragment(loadAsset("render.frag")));
    }
    catch (const gl::GlslProgCompileExc &e) {
        ci::app::console() << e.what() << std::endl;
        quit();
    }
}


void NVidiaComputeParticlesApp::update() {

    auto frame = cameraHandler.frame();
    auto eye_pairs = detector.detect(frame);
    std::vector<ci::vec4> eye_pairs_as_cinder_vectors;
    for (const auto &eye_pair : eye_pairs) {
        auto eye_pair_as_cinder_vector = eye_pair.to_cinder_vectors();
        eye_pairs_as_cinder_vectors.insert(eye_pairs_as_cinder_vectors.end(), eye_pair_as_cinder_vector.begin(),
                                           eye_pair_as_cinder_vector.end());
    }

    ParticleSystem::AppState app_state{
            getMousePos(),
            mCam,
            getWindowSize(),
            mAttractToMouse,
            eye_pairs_as_cinder_vectors
    };
    particle_system_.update(app_state);
}

void NVidiaComputeParticlesApp::draw() {
    gl::clear(ColorA(0.25f, 0.25f, 0.25f, 1.0f));

    if (mReset) {
        mReset = false;
        particle_system_.reset(0.5f);
    }

    gl::setMatrices(mCam);

    // draw particles
    gl::ScopedGlslProg scopedRenderProg(mRenderProg);
    mRenderProg->uniform("spriteSize", 0.015f);

    particle_system_.draw();

    mParams->draw();
}

void NVidiaComputeParticlesApp::keyDown(KeyEvent event) {
    switch (event.getCode()) {
        case KeyEvent::KEY_f:
            setFullScreen(!isFullScreen());
            break;
        case KeyEvent::KEY_ESCAPE:
            if (isFullScreen()) {
                setFullScreen(false);
            } else {
                quit();
            }
            break;
        case KeyEvent::KEY_s:
            mParams->show(!mParams->isVisible());
            break;
        case KeyEvent::KEY_m:
            mAttractToMouse = !mAttractToMouse;
            break;
        case KeyEvent::KEY_r:
            mReset = !mReset;
            break;
    }
}

CINDER_APP(NVidiaComputeParticlesApp, RendererGl(),
           [&](
                   App::Settings *settings
           ) {
               settings->setWindowSize(1280, 720);
           })