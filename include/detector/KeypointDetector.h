//
// Created by benjamin on 27.03.20.
//

#ifndef VISUALS_KEYPOINTDETECTOR_H
#define VISUALS_KEYPOINTDETECTOR_H

#include <opencv4/opencv2/face.hpp>
#include "FaceKeypoints.h"


class KeypointDetector {
public:
    explicit KeypointDetector();

    std::vector<FaceKeypoints> detect_keypoints(const std::vector<cv::Rect> &current_keypoints, const cv::Mat &image) const;

    void draw_detected_keypoints(const std::vector<FaceKeypoints> &faces,
                                 const cv::Mat &frame) const;

private:
    cv::Ptr<cv::face::Facemark> facemark_;

};


#endif //VISUALS_KEYPOINTDETECTOR_H
