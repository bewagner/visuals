#ifndef KEYPOINTDETECTOR_H
#define KEYPOINTDETECTOR_H

#include <opencv4/opencv2/face.hpp>

class KeyPointDetector {
public:
    /// Constructor
    explicit KeyPointDetector();

    /// Detect face key points within a rectangle inside an image
    /// \param face_rectangles Rectangles that contain faces
    /// \param image Image in which we want to detect key points
    /// \return List of face keypoints for each face rectangle
    std::vector<std::vector<cv::Point2f>>
    detect_key_points(const std::vector<cv::Rect> &face_rectangles,
                      const cv::Mat &image) const;

private:
    cv::Ptr<cv::face::Facemark> facemark_;
};


#endif //KEYPOINTDETECTOR_H
