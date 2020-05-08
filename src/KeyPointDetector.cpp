#include "KeyPointDetector.h"
#include <opencv4/opencv2/opencv.hpp>

KeyPointDetector::KeyPointDetector() {
    facemark_ = cv::face::FacemarkLBF::create();
    facemark_->loadModel(KEY_POINT_DETECTION_MODEL);
}

std::vector<std::vector<cv::Point2f> >
KeyPointDetector::detect_key_points(const std::vector<cv::Rect> &face_rectangles, const cv::Mat &image) const {

    cv::InputArray faces_as_input_array(face_rectangles);
    std::vector<std::vector<cv::Point2f> > key_points;
    facemark_->fit(image, faces_as_input_array, key_points);
    return key_points;
}
