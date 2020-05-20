#include "detector/Detector.h"
#include "CameraHandler.h"
#include <opencv4/opencv2/opencv.hpp>


int main(int argc, char **argv) {

    Detector detector;
    CameraHandler cameraHandler;

    while (true) {
        auto result = detector.detect(cameraHandler.next_frame());
        cameraHandler.show_openCV_window(result);
    }
    return 0;
}