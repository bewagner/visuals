//
// Created by benjamin on 04.04.20.
//

#ifndef VISUALS_CAMERAHANDLER_H
#define VISUALS_CAMERAHANDLER_H

#include <opencv4/opencv2/opencv.hpp>
#include <mutex>
#include <thread>

class CameraHandler {
public:
    explicit CameraHandler();

    ~CameraHandler();

    const cv::Mat &frame() const;


    void show_openCV_window() const;


private:
    const int max_number_of_cameras_to_try_ = 10;
    cv::VideoCapture video_capture_;
    cv::Mat frame_;
    mutable std::mutex frame_mutex_;

    void capture_frame_();

    std::thread frame_capture_thread_;
};


#endif //VISUALS_CAMERAHANDLER_H
