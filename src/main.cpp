#include <iostream>
#include "FaceDetector.h"
#include "KeypointDetector.h"
#include <opencv4/opencv2/opencv.hpp>


int main(int argc, char **argv) {


    FaceDetector face_detector;
    KeypointDetector keypoint_detector;


    cv::VideoCapture video_capture;
    if (!video_capture.open(0)) {
        return 0;
    }
    cv::Mat frame;
    while (true) {
        video_capture >> frame;
        if (frame.channels() == 4) {
            cv::cvtColor(frame, frame, cv::COLOR_BGRA2BGR);
        }

        auto detected_faces = face_detector.detect_faces(frame);
        face_detector.draw_rectangles_around_detected_faces(detected_faces, frame);

        auto detected_keypoints = keypoint_detector.detect_keypoints(detected_faces, frame);
        keypoint_detector.draw_detected_keypoints(detected_keypoints, frame);

        imshow("Image", blurred);

        if (cv::waitKey(10) == 27) {
            break;
        }
    }

    video_capture.release();
    cv::destroyAllWindows();

    return 0;
}