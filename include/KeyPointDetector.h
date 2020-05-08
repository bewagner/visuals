#ifndef VISUALS_KEYPOINTDETECTOR_H
#define VISUALS_KEYPOINTDETECTOR_H

#include <opencv4/opencv2/face.hpp>

class KeyPointDetector {
public:
    explicit KeyPointDetector();

    std::vector<std::vector<cv::Point2f>>
    detect_key_points(const std::vector<cv::Rect> &face_rectangles,
                      const cv::Mat &image) const;

private:
    cv::Ptr<cv::face::Facemark> facemark_;
};


#endif //VISUALS_KEYPOINTDETECTOR_H
