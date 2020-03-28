#include <iostream>
#include "FaceDetector.h"
#include "KeypointDetector.h"
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/tracking.hpp>


int main(int argc, char **argv) {


    FaceDetector face_detector;
    KeypointDetector keypoint_detector;


    cv::VideoCapture video_capture;
    if (!video_capture.open(0)) {
        return 0;
    }
    cv::Mat frame, frame_gray;

    unsigned int frame_counter = 0;


    cv::Mat old_frame, old_gray;

    std::vector<cv::Point2f> eyes;
    std::vector<Face> detected_faces;

    while (true) {
        video_capture >> frame;
        if (frame.channels() == 4) {
            cv::cvtColor(frame, frame, cv::COLOR_BGRA2BGR);
        }
        cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);


        if (frame_counter % 80 == 0 || detected_faces.empty()) {


            putText(frame, "Detecting faces", cv::Point(100, 80), cv::FONT_HERSHEY_SIMPLEX, 0.75,
                    cv::Scalar(0, 0, 255), 2);
            auto detected_face_rectangles = face_detector.detect_faces(frame);

            if (!detected_face_rectangles.empty()) {
                detected_faces = keypoint_detector.detect_keypoints(detected_face_rectangles, frame);

                eyes.clear();
                eyes.reserve(detected_faces.size() * 2);
                for (const auto &face : detected_faces) {
                    eyes.emplace_back(face.left_eye_center());
                    eyes.emplace_back(face.right_eye_center());
                }
            }


        } else if (!eyes.empty()) {

            int size = 30;
            std::vector<uchar> status;
            std::vector<float> err;
            std::vector<cv::Point2f> new_eyes;
            cv::TermCriteria criteria = cv::TermCriteria((cv::TermCriteria::COUNT) + (cv::TermCriteria::EPS), 10, 0.03);
            calcOpticalFlowPyrLK(old_gray, frame_gray, eyes, new_eyes, status, err, cv::Size(size, size), 2, criteria);
            for (int i = 0; i < std::min(eyes.size(), new_eyes.size()); ++i) {
                if (status[i] == 1) {
                    eyes[i] = new_eyes[i];
                }
            }

        }

        old_frame = frame.clone();
        old_gray = frame_gray.clone();


        for (const auto &eye : eyes) {
            cv::circle(frame, eye, 8, cv::Scalar(0, 255, 0), -1);
        }
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