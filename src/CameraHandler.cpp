//
// Created by benjamin on 04.04.20.
//

#include "CameraHandler.h"

CameraHandler::CameraHandler() {

    for (int i = 0; i < max_number_of_cameras_to_try_; ++i) {
        if (video_capture_.open(i)) {
            break;
        }
    }
    if (!video_capture_.isOpened()) {
        throw std::invalid_argument("Video capture could not find camera.");
    }
}

cv::Mat CameraHandler::next_frame() {
    cv::Mat frame;
    video_capture_ >> frame;
    return frame;
}
