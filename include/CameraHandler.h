//
// Created by benjamin on 04.04.20.
//

#ifndef VISUALS_CAMERAHANDLER_H
#define VISUALS_CAMERAHANDLER_H

#include <opencv4/opencv2/opencv.hpp>

class CameraHandler {
public:
    explicit CameraHandler();

    ~CameraHandler();

    const cv::Mat &next_frame();

    template<typename T>
    void show_openCV_window(const std::vector<T> &objects);

    void show_openCV_window();


private:
    const int max_number_of_cameras_to_try_ = 10;
    cv::VideoCapture video_capture_;
    cv::Mat frame_;
};


#endif //VISUALS_CAMERAHANDLER_H
