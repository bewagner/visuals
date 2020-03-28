#include <iostream>
#include "FaceDetector.h"
#include "KeypointDetector.h"
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/tracking.hpp>


void recreate_tracker(cv::Ptr<cv::Tracker> & tracker) {
    tracker = cv::TrackerMedianFlow::create();
}

int main(int argc, char **argv) {


    FaceDetector face_detector;
    KeypointDetector keypoint_detector;
    cv::Ptr<cv::Tracker> tracker;
    recreate_tracker(tracker);


    cv::VideoCapture video_capture;
    if (!video_capture.open(0)) {
        return 0;
    }
    cv::Mat frame;

    unsigned int frame_counter = 0;
    std::vector<cv::Rect> detected_faces;
    std::vector<Face> detected_keypoints;
    cv::Rect2d face;
    bool tracking_error_in_last_frame = false;

    while (true) {
        video_capture >> frame;
        if (frame.channels() == 4) {
            cv::cvtColor(frame, frame, cv::COLOR_BGRA2BGR);
        }

        if (frame_counter % 60 == 0 || detected_faces.empty() || tracking_error_in_last_frame) {


            putText(frame, "Detecting faces", cv::Point(100, 80), cv::FONT_HERSHEY_SIMPLEX, 0.75,
                    cv::Scalar(0, 0, 255), 2);
            detected_faces = face_detector.detect_faces(frame);

            if (detected_faces.empty()) {
                continue;
            }

            tracking_error_in_last_frame = false;
            face = detected_faces.front();
            recreate_tracker(tracker);
            tracker->init(frame, face);


        } else {
            bool ok = tracker->update(frame, face);
            if (!ok) {
                tracking_error_in_last_frame = true;
                putText(frame, "Tracking failure detected", cv::Point(100, 80), cv::FONT_HERSHEY_SIMPLEX, 0.75,
                        cv::Scalar(0, 0, 255), 2);
            }
        }


        face_detector.draw_rectangles_around_detected_faces({face}, frame);
        auto keypoints = keypoint_detector.detect_keypoints({face}, frame);
        keypoint_detector.draw_detected_keypoints(keypoints, frame);
        imshow("Image", frame);


        frame_counter++;

        // Esc
        if (cv::waitKey(10) == 27) {
            break;
        }
    }

    video_capture.release();
    cv::destroyAllWindows();

    return 0;
}