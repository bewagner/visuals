//
// Created by benjamin on 27.03.20.
//

#ifndef VISUALS_KEYPOINTDETECTOR_H
#define VISUALS_KEYPOINTDETECTOR_H

#include <opencv4/opencv2/face.hpp>

class KeypointDetector {
public:
    explicit KeypointDetector();

    std::vector<std::vector<cv::Point2f>>
    detect_keypoints(const std::vector<cv::Rect> &faces, const cv::Mat &image) const;

    void draw_detected_keypoints(const std::vector<std::vector<cv::Point2f>> &keypoints_for_all_faces,
                                 const cv::Mat &frame) const;

private:
    cv::Ptr<cv::face::Facemark> facemark_;

};


#endif //VISUALS_KEYPOINTDETECTOR_H
