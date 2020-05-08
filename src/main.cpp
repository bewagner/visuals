#include <opencv4/opencv2/opencv.hpp>
#include "FaceDetector.h"
#include "KeyPointDetector.h"

int main(int argc, char **argv) {

    cv::VideoCapture video_capture;
    if (!video_capture.open(0)) {
        return 0;
    }

    FaceDetector face_detector;
    KeyPointDetector keypoint_detector;

    cv::Mat frame;
    while (true) {
        video_capture >> frame;
        auto rectangles = face_detector
                .detect_face_rectangles(frame);

        auto keypoint_faces = keypoint_detector
                .detect_key_points(rectangles, frame);

        const auto red = cv::Scalar(0, 0, 255);
        for (const auto &face :keypoint_faces) {
            for (const cv::Point2f &keypoint : face) {
                cv::circle(frame, keypoint,
                           8, red, -1);
            }
        }

        imshow("Image", frame);
        const int esc_keycode = 27;
        if (cv::waitKey(10) == esc_keycode) {
            break;
        }
    }
    cv::destroyAllWindows();
    video_capture.release();
    return 0;
}