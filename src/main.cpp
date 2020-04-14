#include <opencv4/opencv2/opencv.hpp>
#include "FaceDetector.h"

int main(int argc, char **argv) {

    cv::VideoCapture video_capture;
    if (!video_capture.open(0)) {
        return 0;
    }

    FaceDetector face_detector;

    cv::Mat frame;
    while (true) {
        video_capture >> frame;

        auto rectangles = face_detector.detect_face_rectangles(frame);
        cv::Scalar color(0, 105, 205);
        for(const auto & r : rectangles){
            cv::rectangle(frame, r, color, 4);
        }


        imshow("Image", frame);
        int esc_key = 27;
        if (cv::waitKey(10) == esc_key) {
            break;
        }
    }

    cv::destroyAllWindows();
    video_capture.release();

    return 0;
}