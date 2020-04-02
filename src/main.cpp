#include "detector/Detector.h"
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

        auto eye_pairs = detector.detect(frame);

        for (const auto &eye_pair : eye_pairs) {
            eye_pair.draw(frame);
            std::cout << eye_pair.distance_between_eyes() << "\n";
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