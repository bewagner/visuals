#include <FaceKeypoints.h>
#include <utility>
#include <numeric>

template<typename T>
std::vector<T> vector_subset(std::vector<T> v, int index_begin, int index_end) {
    if (index_begin < 0 || index_begin > index_end - 1 || index_begin >= v.size() - 1 || index_end > v.size() - 1) {
        throw std::invalid_argument("Indexing error");
    }

    auto first = v.begin() + index_begin;
    auto last = v.begin() + index_end + 1;

    return {first, last};
}

cv::Point2f mean_point(const std::vector<cv::Point2f> &points) {
    if (points.empty()) {
        return {0, 0};
    }

    cv::Point2f summed_points = std::accumulate(points.begin(), points.end(), cv::Point2f(0.f, 0.f));
    return summed_points / static_cast<float>(points.size());
}

FaceKeypoints::FaceKeypoints(std::vector<cv::Point2f> face_keypoints) : keypoints(std::move(face_keypoints)) {
    int number_of_points_opencv_keypoint_detector_detects = 68;
    if (keypoints.size() != number_of_points_opencv_keypoint_detector_detects) {
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

std::vector<cv::Point2f> FaceKeypoints::right_eye() const {
    return vector_subset(keypoints, 36, 41);
}

std::vector<cv::Point2f> FaceKeypoints::left_eye() const {
    return vector_subset(keypoints, 42, 47);
}
