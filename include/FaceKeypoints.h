//
// Created by benjamin on 27.03.20.
//

#ifndef VISUALS_FACEKEYPOINTS_H
#define VISUALS_FACEKEYPOINTS_H

#include <vector>
#include <opencv4/opencv2/opencv.hpp>

struct FaceKeypoints {

    explicit FaceKeypoints(std::vector<cv::Point2f> all_points);

    cv::Point2f left_eye_center() const;

    cv::Point2f right_eye_center() const;

    cv::Point2f mouth_center() const;


    std::vector<cv::Point2f> jawline() const;
    std::vector<cv::Point2f> right_eyebrow() const;
    std::vector<cv::Point2f> left_eyebrow() const;
    std::vector<cv::Point2f> nose() const;
    std::vector<cv::Point2f> right_eye() const;
    std::vector<cv::Point2f> left_eye() const;
    std::vector<cv::Point2f> mouth() const;


    std::vector<cv::Point2f> keypoints;
};


#endif //VISUALS_FACEKEYPOINTS_H
