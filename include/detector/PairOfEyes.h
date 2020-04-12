//
// Created by benjamin on 02.04.20.
//

#ifndef VISUALS_PAIROFEYES_H
#define VISUALS_PAIROFEYES_H

#include <opencv4/opencv2/opencv.hpp>
#include <utility>
#include <vector>
#include "cinder/gl/gl.h"

class PairOfEyes {
public:

    explicit PairOfEyes(cv::Point2f left, cv::Point2f right) : left_eye(std::move(left)), right_eye(std::move(right)) {}

    cv::Point2f left_eye;
    cv::Point2f right_eye;

    void draw(const cv::Mat &image) const {
        for (const auto &eye : {left_eye, right_eye}) {
            cv::circle(image, eye, 8, cv::Scalar(0, 255, 0), -1);
        }
    }

    double distance_between_eyes() const {
        return cv::norm(left_eye - right_eye);
    }

    std::vector<ci::vec4> to_cinder_vectors() const {
        // TODO Reverse x direction here
        std::vector<ci::vec4> result;
        result.emplace_back(ci::vec4(left_eye.x, left_eye.y, 0., distance_between_eyes()));
        result.emplace_back(ci::vec4(right_eye.x, right_eye.y, 0., distance_between_eyes()));
        return result;
    }
};


#endif //VISUALS_PAIROFEYES_H
