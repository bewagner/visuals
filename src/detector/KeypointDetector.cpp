//
// Created by benjamin on 27.03.20.
//

#include <detector/KeypointDetector.h>

#include "detector/KeypointDetector.h"
#include <opencv4/opencv2/opencv.hpp>

KeypointDetector::KeypointDetector() {
    facemark_ = cv::face::FacemarkLBF::create();
    facemark_->loadModel(KEYPOINT_DETECTION_MODEL);

}

std::vector<FaceKeypoints>
KeypointDetector::detect_keypoints(const std::vector<cv::Rect> &face_rectangles, const cv::Mat &image) const {

    cv::InputArray faces_as_input_array(face_rectangles);
    std::vector<std::vector<cv::Point2f> > keypoints;
    facemark_->fit(image, faces_as_input_array, keypoints);

    std::vector<FaceKeypoints> faces;
    std::transform(keypoints.begin(), keypoints.end(), std::back_inserter(faces), [](const auto &current_keypoints) {
        return FaceKeypoints(current_keypoints);
    });

    return faces;
}

void KeypointDetector::draw_detected_keypoints(const std::vector<FaceKeypoints> &faces,
                                               const cv::Mat &frame) const {
    cv::Scalar red(0, 0, 255);
    for (const auto &face : faces) {
        cv::circle(frame, face.left_eye_center(), 8, red, -1);
        cv::circle(frame, face.right_eye_center(), 8, red, -1);
    }
}
