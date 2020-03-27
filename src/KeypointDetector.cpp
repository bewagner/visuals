//
// Created by benjamin on 27.03.20.
//

#include <KeypointDetector.h>

#include "KeypointDetector.h"

KeypointDetector::KeypointDetector() {
    facemark_ = cv::face::FacemarkLBF::create();
    facemark_->loadModel(KEYPOINT_DETECTION_MODEL);

}

std::vector<std::vector<cv::Point2f>>
KeypointDetector::detect_keypoints(const std::vector<cv::Rect> &faces, const cv::Mat &image) const {

    cv::InputArray faces_as_input_array(faces);
    std::vector<std::vector<cv::Point2f> > keypoints;
    facemark_->fit(image, faces_as_input_array, keypoints);

    return keypoints;
}

void KeypointDetector::draw_detected_keypoints(const std::vector<std::vector<cv::Point2f>> &keypoints_for_all_faces,
                                               const cv::Mat &frame) const {
    for (const auto &individual_face_keypoints : keypoints_for_all_faces) {
        cv::face::drawFacemarks(frame, individual_face_keypoints, cv::Scalar(0, 255, 255));
    }
}
