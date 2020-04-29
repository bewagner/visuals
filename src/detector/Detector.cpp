//
// Created by benjamin on 01.04.20.
//

#include "detector/Detector.h"


std::vector<PairOfEyes> Detector::detect(const cv::Mat &frame) {

    cv::Mat frame_grayscale;


    if (frame.channels() == 4) {
        cv::cvtColor(frame, frame, cv::COLOR_BGRA2BGR);
    }
    cv::cvtColor(frame, frame_grayscale, cv::COLOR_BGR2GRAY);


    if (frame_counter_ > frequency_of_detection_frames_) {
        frame_counter_ = 0;
        detected_eye_pairs_ = run_detection(frame);
    } else {
        detected_eye_pairs_ = run_tracking(frame_grayscale, last_frame_grayscale_, detected_eye_pairs_);
    }

    ++frame_counter_;
    last_frame_grayscale_ = frame_grayscale.clone();
    return detected_eye_pairs_;
}

std::vector<cv::Point2f> eye_pairs_to_points(const std::vector<PairOfEyes> &eye_pairs) {

    std::vector<cv::Point2f> points;
    points.reserve(eye_pairs.size() * 2);

    for (const auto &eye_pair : eye_pairs) {
        points.emplace_back(eye_pair.left_eye);
        points.emplace_back(eye_pair.right_eye);
    }
    return points;
}


std::vector<PairOfEyes> points_to_eye_pair(const std::vector<cv::Point2f> &points) {

    std::vector<PairOfEyes> eye_pairs;

    if (points.size() % 2 != 0) {
        return eye_pairs;
    }

    eye_pairs.reserve(points.size() / 2);


    for (size_t i = 0; i < points.size(); i += 2) {
        eye_pairs.emplace_back(points[i], points[i + 1]);
    }
    return eye_pairs;
}


std::vector<PairOfEyes>
Detector::run_tracking(const cv::Mat &current_frame_grayscale, const cv::Mat &last_frame_grayscale,
                       const std::vector<PairOfEyes> &old_eyes) const {
    if (old_eyes.empty()) {
        return old_eyes;
    }

    const int filter_size = 30;
    std::vector<uchar> status;
    std::vector<float> err;

    cv::TermCriteria termination_criteria = cv::TermCriteria((cv::TermCriteria::COUNT) + (cv::TermCriteria::EPS), 10,
                                                             0.03);
    std::vector<cv::Point2f> old_points = eye_pairs_to_points((old_eyes));
    std::vector<cv::Point2f> new_points;


    calcOpticalFlowPyrLK(last_frame_grayscale, current_frame_grayscale, old_points, new_points, status, err,
                         cv::Size(filter_size, filter_size), 2, termination_criteria);

    auto number_of_points = static_cast<unsigned int>(std::min(old_points.size(), new_points.size()));
    for (unsigned int i = 0; i < number_of_points; ++i) {
        if (status[i] != 1) {
            new_points[i] = old_points[i];
        }
    }

    return points_to_eye_pair(new_points);
}

std::vector<PairOfEyes> Detector::run_detection(const cv::Mat &frame) {
    std::vector<PairOfEyes> detected_eyes;

    auto detected_face_rectangles = face_rectangle_detector.detect_face_rectangles(frame);
    if (!detected_face_rectangles.empty()) {
        auto detected_face_keypoints = face_keypoint_detector.detect_keypoints(detected_face_rectangles, frame);

        detected_eyes.reserve(detected_face_keypoints.size());
        for (const auto &face : detected_face_keypoints) {
            detected_eyes.emplace_back(face.left_eye_center(), face.right_eye_center());
        }
    }

    return detected_eyes;
}

