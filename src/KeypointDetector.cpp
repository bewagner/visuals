#include "KeypointDetector.h"
#include <opencv4/opencv2/opencv.hpp>

KeypointDetector::KeypointDetector() {
    facemark_ = cv::face::FacemarkLBF::create();
    facemark_->loadModel(KEYPOINT_DETECTION_MODEL);
}

std::vector<std::vector<cv::Point2f> >
KeypointDetector::detect_keypoints(const std::vector<cv::Rect> &face_rectangles, const cv::Mat &image) const {

    cv::InputArray faces_as_input_array(face_rectangles);
    std::vector<std::vector<cv::Point2f> > keypoints;
    facemark_->fit(image, faces_as_input_array, keypoints);
    return keypoints;
}
