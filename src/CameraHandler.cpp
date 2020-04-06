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

const cv::Mat &CameraHandler::next_frame() {
    video_capture_ >> frame_;
    return frame_;
}

template<typename T>
void CameraHandler::show_openCV_window(const std::vector<T> &objects) {
    for (const auto &object : objects) {
        object.draw(frame_);
    }
    show_openCV_window();
}

void CameraHandler::show_openCV_window() {
    cv::imshow("Frame", frame_);

    if (cv::waitKey(1) == 27) {
        cv::destroyAllWindows();
        video_capture_.release();
    }
}
