//
// Created by benjamin on 27.03.20.
//

#include <detector/FaceKeypoints.h>
#include <utility>
#include <exception>
#include <numeric>

template<typename T>
std::vector<T> vector_subset(std::vector<T> v, int index_begin, int index_end) {
    auto first = v.begin() + index_begin;
    auto last = v.begin() + index_end + 1;

    std::vector<T> subset(first, last);
    return subset;
}


cv::Point2f mean_point(const std::vector<cv::Point2f> &points) {
    cv::Point2f summed_points = std::accumulate(points.begin(), points.end(), cv::Point2f(0.f, 0.f));
    return summed_points / static_cast<float>(points.size());
}

FaceKeypoints::FaceKeypoints(std::vector<cv::Point2f> all_points) : keypoints(std::move(all_points)) {
    int number_of_points_opencv_keypoint_detector_detects = 68;
    int current_number_of_keypoints = static_cast<int>(keypoints.size());
    if (current_number_of_keypoints != number_of_points_opencv_keypoint_detector_detects) {
        throw std::invalid_argument("Number of keypoints to construct face was not " +
                                    std::to_string(number_of_points_opencv_keypoint_detector_detects));
    }
}

cv::Point2f FaceKeypoints::left_eye_center() const {
    return mean_point(left_eye());
}

cv::Point2f FaceKeypoints::right_eye_center() const {
    return mean_point(right_eye());
}

cv::Point2f FaceKeypoints::mouth_center() const {
    return mean_point(mouth());
}

std::vector<cv::Point2f> FaceKeypoints::jawline() const {
    return vector_subset(keypoints, 0, 16);
}

std::vector<cv::Point2f> FaceKeypoints::right_eyebrow() const {
    return vector_subset(keypoints, 17, 21);
}

std::vector<cv::Point2f> FaceKeypoints::left_eyebrow() const {
    return vector_subset(keypoints, 22, 26);
}

std::vector<cv::Point2f> FaceKeypoints::nose() const {
    return vector_subset(keypoints, 27, 35);
}

std::vector<cv::Point2f> FaceKeypoints::right_eye() const {
    return vector_subset(keypoints, 36, 41);
}

std::vector<cv::Point2f> FaceKeypoints::left_eye() const {
    return vector_subset(keypoints, 42, 47);
}

std::vector<cv::Point2f> FaceKeypoints::mouth() const {
    return vector_subset(keypoints, 48, 67);
}
