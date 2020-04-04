//
// Created by benjamin on 04.04.20.
//

#ifndef VISUALS_CAMERAHANDLER_H
#define VISUALS_CAMERAHANDLER_H

#include <opencv4/opencv2/opencv.hpp>

class CameraHandler {
public:
    explicit CameraHandler();
    cv::Mat next_frame();
private:
    const int max_number_of_cameras_to_try_ = 10;
    cv::VideoCapture video_capture_;
};


#endif //VISUALS_CAMERAHANDLER_H
