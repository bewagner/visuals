#include <opencv4/opencv2/opencv.hpp>
#include "FaceDetector.h"
#include "KeypointDetector.h"

int main(int argc, char **argv) {

    cv::VideoCapture video_capture;
    if (!video_capture.open(0)) {
        return 0;
    }

    FaceDetector face_detector;
    KeypointDetector keypoint_detector;

    cv::Mat frame;
    while (true) {
        video_capture >> frame;
        auto rectangles = face_detector.detect_face_rectangles(frame);
        auto face_keypoints_vector = keypoint_detector.detect_keypoints(rectangles, frame);
        for (const auto &face_keypoints :face_keypoints_vector) {
            cv::circle(frame, face_keypoints.left_eye_center(), 8, cv::Scalar(0, 0, 255), -1);
            cv::circle(frame, face_keypoints.right_eye_center(), 8, cv::Scalar(0, 0, 255), -1);
        }

        imshow("Image", frame);
        if (cv::waitKey(10) == 27) { // Stop showing the image when you press 'Esc'
            break;
        }
    }
    cv::destroyAllWindows();
    video_capture.release();
    return 0;
}