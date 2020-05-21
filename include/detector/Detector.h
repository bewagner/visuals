//
// Created by benjamin on 01.04.20.
//

#ifndef VISUALS_DETECTOR_H
#define VISUALS_DETECTOR_H

#include <vector>
#include <opencv4/opencv2/opencv.hpp>
#include "detector/FaceDetector.h"
#include "detector/KeypointDetector.h"
#include "detector/FaceKeypoints.h"
#include "detector/PairOfEyes.h"

class Detector {
public:
    /// TODO
    /// \return
    std::vector<PairOfEyes> detect(const cv::Mat &frame);

private:
    /// TODO
    const int frequency_of_detection_frames_{240};
    /// TODO
    int frame_counter_{0};
    /// TODO
    cv::Mat last_frame_grayscale_;
    /// TODO
    std::vector<PairOfEyes> detected_eye_pairs_;
    /// TODO
    FaceDetector face_rectangle_detector;
    /// TODO
    KeypointDetector face_keypoint_detector;


    std::vector<PairOfEyes> run_detection(const cv::Mat &frame);

    std::vector<PairOfEyes> run_tracking(const cv::Mat &current_frame_grayscale, const cv::Mat &last_frame_grayscale,
                                          const std::vector<PairOfEyes> &old_eyes) const;
};


#endif //VISUALS_DETECTOR_H
