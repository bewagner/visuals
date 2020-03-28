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

    // Cinder will call 'mouseDrag' when the user moves the mouse while holding one of its buttons.
    // See also: mouseMove, mouseDown, mouseUp and mouseWheel.
    void mouseDrag(MouseEvent event) override;

    void keyDown(KeyEvent event) override;

    void draw() override;

    void update() override;

private:
    std::vector<vec2> points_;
    cv::VideoCapture video_capture_;
    cv::Mat frame_;



    FaceDetector face_detector;
    KeypointDetector keypoint_detector;
};

void prepareSettings(BasicApp::Settings *settings) {
    settings->setMultiTouchEnabled(false);
}

void BasicApp::mouseDrag(MouseEvent event) {
    // Store the current mouse position in the list.
    points_.emplace_back(event.getPos());
}

void BasicApp::keyDown(KeyEvent event) {

    if (event.getChar() == 'f') {
        // Toggle full screen when the user presses the 'f' key.
        setFullScreen(!isFullScreen());
    } else if (event.getCode() == KeyEvent::KEY_SPACE) {
        // Clear the list of points when the user presses the space bar.
        points_.clear();
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
    gl::clear(Color::gray(0.1f));

    // Set the current draw color to orange by setting values for
    // red, green and blue directly. Values range from 0 to 1.
    // See also: gl::ScopedColor
    gl::color(1.0f, 0.5f, 0.25f);

    // We're going to draw a line through all the points in the list
    // using a few convenience functions: 'begin' will tell OpenGL to
    // start constructing a line strip, 'vertex' will add a point to the
    // line strip and 'end' will execute the draw calls on the GPU.
    gl::begin(GL_LINE_STRIP);
    for (const vec2 &point : points_) {
        gl::vertex(point);
    }
    gl::end();
}

void BasicApp::update() {
    console() << getAverageFps() << std::endl;

    video_capture_ >> frame_;
    if (frame_.empty()) {
        return;
    }
    if (frame_.channels() == 4) {
        cv::cvtColor(frame_, frame_, cv::COLOR_BGRA2BGR);
    }

    auto detected_faces = face_detector.detect_faces(frame_);
    face_detector.draw_rectangles_around_detected_faces(detected_faces, frame_);

    auto detected_keypoints = keypoint_detector.detect_keypoints(detected_faces, frame_);
    keypoint_detector.draw_detected_keypoints(detected_keypoints, frame_);


    cv::imshow("Frame", frame);

    // Esc
    if (cv::waitKey(1) == 27) {
        video_capture_.release();
        cv::destroyAllWindows();
    }
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
}


CINDER_APP(BasicApp, RendererGl, prepareSettings)


// TODO
// - Test opencv cinder block