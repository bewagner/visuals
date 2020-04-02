//
// Created by benjamin on 01.04.20.
//

#include "detector/detector.h"

std::vector<cv::Point2f> Detector::detect(const cv::Mat &frame) {

    cv::Mat frame_grayscale;


    if (frame.channels() == 4) {
        cv::cvtColor(frame, frame, cv::COLOR_BGRA2BGR);
    }
    cv::cvtColor(frame, frame_grayscale, cv::COLOR_BGR2GRAY);


    if (frame_counter_ > frequency_of_detection_frames_ || detected_eyes_.empty()) {
        frame_counter_ = 0;
        detected_eyes_ = run_detection(frame);
    } else {
        frame_counter_++;
        detected_eyes_ = run_tracking(frame_grayscale, last_frame_grayscale_, detected_eyes_);
    }

    last_frame_grayscale_ = frame_grayscale.clone();
    return detected_eyes_;
}

std::vector<cv::Point2f>
Detector::run_tracking(const cv::Mat &current_frame_grayscale, const cv::Mat &last_frame_grayscale,
                       const std::vector<cv::Point2f> &old_eyes) const {
    if (old_eyes.empty()) {
        return old_eyes;
    }

    const int filter_size = 30;
    std::vector<uchar> status;
    std::vector<float> err;
    std::vector<cv::Point2f> new_eyes;
    cv::TermCriteria termination_criteria = cv::TermCriteria((cv::TermCriteria::COUNT) + (cv::TermCriteria::EPS), 10,
                                                             0.03);
    calcOpticalFlowPyrLK(last_frame_grayscale, current_frame_grayscale, old_eyes, new_eyes, status, err,
                         cv::Size(filter_size, filter_size), 2, termination_criteria);
    for (int i = 0; i < std::min(old_eyes.size(), new_eyes.size()); ++i) {
        if (status[i] != 1) {
            new_eyes[i] = old_eyes[i];
        }
    }
    return new_eyes;
}

std::vector<cv::Point2f> Detector::run_detection(const cv::Mat &frame) {
    std::vector<cv::Point2f> detected_eyes;

    auto detected_face_rectangles = face_rectangle_detector.detect_face_rectangles(frame);
    if (!detected_face_rectangles.empty()) {
        auto detected_face_keypoints = face_keypoint_detector.detect_keypoints(detected_face_rectangles, frame);


        detected_eyes.reserve(detected_face_keypoints.size() * 2);
        for (const auto &face : detected_face_keypoints) {
            detected_eyes.emplace_back(face.left_eye_center());
            detected_eyes.emplace_back(face.right_eye_center());
        }
    }

    return detected_eyes;
}

