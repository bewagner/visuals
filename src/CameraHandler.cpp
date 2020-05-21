//
// Created by benjamin on 04.04.20.
//
#include "CameraHandler.h"
#include <chrono>

CameraHandler::CameraHandler() {

    for (int i = 0; i < max_number_of_cameras_to_try_; ++i) {
        if (video_capture_.open(i)) {
            break;
        }
    }
    if (!video_capture_.isOpened()) {
        throw std::invalid_argument("Video capture could not find camera.");
    }

    frame_capture_thread_ = std::thread(&CameraHandler::capture_frame_, this);
}

const cv::Mat &CameraHandler::frame() const {
    std::lock_guard<std::mutex> frame_lock(frame_mutex_);
    return frame_;
}


void CameraHandler::show_openCV_window() const {
    cv::imshow("Frame", frame_);

    if (cv::waitKey(1) == 27) {
        cv::destroyAllWindows();
    }
}

CameraHandler::~CameraHandler() {
    video_capture_.release();
}

void CameraHandler::capture_frame_() {
    while (true) {
        cv::Mat local_frame;
        video_capture_ >> local_frame;
        {
            std::lock_guard<std::mutex> frame_lock(frame_mutex_);
            frame_ = local_frame.clone();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}
