#include <iostream>
#include "detector/Detector.h"
#include <opencv4/opencv2/opencv.hpp>
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Ubo.h"


using namespace ci;
using namespace ci::app;

class BasicApp : public App {
public:

    BasicApp();


    void keyDown(KeyEvent event) override;

    void draw() override;

    void update() override;

private:
    void showOpenCVWindow(const cv::Mat &frame);

    Detector detector_;
    std::vector<PairOfEyes> eye_pairs_;
    cv::VideoCapture video_capture_;
    gl::GlslProgRef shader;

    const int max_number_of_eye_pairs_ = 10;
    gl::UboRef eye_positions_ubo_;
};

void prepareSettings(BasicApp::Settings *settings) {
    settings->setMultiTouchEnabled(false);
}


void BasicApp::keyDown(KeyEvent event) {

    if (event.getChar() == 'f') {
        // Toggle full screen when the user presses the 'f' key.
        setFullScreen(!isFullScreen());
    } else if (event.getCode() == KeyEvent::KEY_ESCAPE) {
        // Exit full screen, or quit the application, when the user presses the ESC key.
        if (isFullScreen()) {
            setFullScreen(false);
        } else {
            quit();
        }
    }
}

void BasicApp::draw() {
    // Clear the contents of the window. This call will clear
    // both the color and depth buffers.
    gl::clear();
    gl::ScopedGlslProg s(shader);

    vec2 face_position = vec2(-1.);
    if (!eye_pairs_.empty()) {
        auto left_eye = eye_pairs_.front().left_eye;

        face_position.x = left_eye.x / static_cast<float>(getWindowWidth());
        face_position.y = left_eye.y / static_cast<float>(getWindowHeight());
    }

    shader->uniform("facePosition", face_position);
    shader->uniform("uResolution", vec2(getWindowSize()));
    gl::drawSolidRect(getWindowBounds());

}

void BasicApp::update() {
    cv::Mat frame;
    video_capture_ >> frame;
    eye_pairs_ = detector_.detect(frame);


    auto *pair_ubo = (PairOfEyes *) eye_positions_ubo_->mapWriteOnly();
    int pair_counter = 0;
    for (const auto &pair_cpu : eye_pairs_) {
        *pair_ubo = pair_cpu.normalized_to_window_size(getWindowWidth(), getWindowHeight());
        ++pair_ubo;
        ++pair_counter;
    }
    while (pair_counter < max_number_of_eye_pairs_) {
        pair_ubo->left_eye = {-1., -1.};
        pair_ubo->right_eye = {-1., -1.};
        ++pair_ubo;
        ++pair_counter;
    }
    eye_positions_ubo_->unmap();
}

BasicApp::BasicApp() {
    const int max_number_of_cameras_to_try = 10;
    for (int i = 0; i < max_number_of_cameras_to_try; ++i) {
        if (video_capture_.open(i)) {
            break;
        }
    }
    if (!video_capture_.isOpened()) {
        throw std::invalid_argument("Video capture could not find camera.");
    }

    shader = gl::GlslProg::create(loadAsset("shader.vert"), loadAsset("shader.frag"));
    eye_positions_ubo_ = gl::Ubo::create(sizeof(PairOfEyes) * max_number_of_eye_pairs_, eye_pairs_.data());
    eye_positions_ubo_->bindBufferBase(0);
}


void BasicApp::showOpenCVWindow(const cv::Mat &frame) {

    for (const auto &eye_pair : eye_pairs_) {
        eye_pair.draw(frame);
    }

    cv::imshow("Frame", frame);

    if (cv::waitKey(1) == 27) {
        video_capture_.release();
        cv::destroyAllWindows();
    }
}

CINDER_APP(BasicApp, RendererGl, prepareSettings)


// TODO
// - Test opencv cinder block