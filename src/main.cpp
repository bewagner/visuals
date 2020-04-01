#include <iostream>
#include "FaceDetector.h"
#include "KeypointDetector.h"
#include <opencv4/opencv2/opencv.hpp>
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "CinderOpenCV.h"

using namespace ci;
using namespace ci::app;

class BasicApp : public App {
public:

    BasicApp();

    void mouseDrag(MouseEvent event) override;

    void keyDown(KeyEvent event) override;

    void draw() override;

    void update() override;

private:
    void showOpenCVWindow();
    std::vector<Face> faceDetectionPipeline();

    cv::VideoCapture video_capture_;
    cv::Mat frame_;
    FaceDetector face_detector;
    KeypointDetector keypoint_detector;
    gl::GlslProgRef shader;
};

void prepareSettings(BasicApp::Settings *settings) {
    settings->setMultiTouchEnabled(false);
}

void BasicApp::mouseDrag(MouseEvent event) {
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
    auto detected_faces = faceDetectionPipeline();

    // Clear the contents of the window. This call will clear
    // both the color and depth buffers.
    gl::clear();
    gl::ScopedGlslProg s(shader);

    vec2 face_position = vec2(-1.);
    if (!detected_faces.empty()) {
        auto left_eye = detected_faces.front().left_eye();

        face_position.x = left_eye[0].x / getWindowWidth();
        face_position.y = left_eye[1].y / getWindowHeight();
        std::cout << face_position << std::endl;
    }




    shader->uniform("facePosition", face_position);
    shader->uniform("uResolution", vec2(getWindowSize()));
    gl::drawSolidRect(getWindowBounds());
    showOpenCVWindow();
}

void BasicApp::update() {


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
}

std::vector<Face> BasicApp::faceDetectionPipeline() {
    video_capture_ >> frame_;
    if (frame_.empty()) {
        return std::vector<Face>();
    }
    if (frame_.channels() == 4) {
        cv::cvtColor(frame_, frame_, cv::COLOR_BGRA2BGR);
    }

    auto detected_faces = face_detector.detect_faces(frame_);
//    face_detector.draw_rectangles_around_detected_faces(detected_faces, frame_);

    auto detected_keypoints = keypoint_detector.detect_keypoints(detected_faces, frame_);
    keypoint_detector.draw_detected_keypoints(detected_keypoints, frame_);

    return detected_keypoints;

}

void BasicApp::showOpenCVWindow() {

    cv::imshow("Frame", frame_);

    // Esc
    if (cv::waitKey(1) == 27) {
        video_capture_.release();
        cv::destroyAllWindows();
    }
}

CINDER_APP(BasicApp, RendererGl, prepareSettings)


// TODO
// - Test opencv cinder block