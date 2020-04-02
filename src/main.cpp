#include "detector/detector.h"
#include <opencv4/opencv2/opencv.hpp>

int main(int argc, char **argv) {
    cv::VideoCapture video_capture;
    if (!video_capture.open(0)) {
        return 0;
    }
    cv::Mat frame;
    Detector detector;
    while (true) {
        video_capture >> frame;

        auto eyes = detector.detect(frame);

        for (const auto &eye : eyes) {
            cv::circle(frame, eye, 8, cv::Scalar(0, 255, 0), -1);
        }
        imshow("Image", frame);
        if (cv::waitKey(10) == 27) {
            break;
        }
    }
    video_capture.release();
    cv::destroyAllWindows();

    return 0;
}