#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char **argv) {
    VideoCapture video_capture;
    if (!video_capture.open(0)) {
        return 0;
    }

    while (true) {
        Mat frame;
        video_capture >> frame;
        if (frame.empty()) {
            break;
        }

        imshow("Image", frame);

        if (waitKey(10) == 27) {
            break;
        }

    }

    video_capture.release();

    destroyAllWindows();


    return 0;
}