#ifndef VISUALS_FACEKEYPOINTS_H
#define VISUALS_FACEKEYPOINTS_H

#include <vector>
#include <opencv4/opencv2/opencv.hpp>

struct FaceKeypoints {

    explicit FaceKeypoints(std::vector<cv::Point2f> face_keypoints);

    cv::Point2f left_eye_center() const;

    cv::Point2f right_eye_center() const;

    std::vector<cv::Point2f> right_eye() const;

    std::vector<cv::Point2f> left_eye() const;

    std::vector<cv::Point2f> keypoints;
};


#endif //VISUALS_FACEKEYPOINTS_H
