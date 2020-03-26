#include <iostream>

#include "FaceDetector.h"

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/face.hpp>


int main(int argc, char **argv) {


    FaceDetector face_detector;


    // Create an instance of Facemark
    cv::Ptr<cv::face::Facemark> facemark = cv::face::FacemarkLBF::create();
    // Load landmark detector
    facemark->loadModel("lbfmodel.yaml");

//    cv::Conf config("lbpcascade_frontalface_improved.xml");



    cv::VideoCapture video_capture;
    if (!video_capture.open(0)) {
        return 0;
    }
    cv::Mat frame;
    while (true) {
        video_capture >> frame;
        if (frame.channels() == 4) {
            cv::cvtColor(frame, frame, cv::COLOR_BGRA2BGR);
        }


        auto detected_faces = face_detector.detect_faces(frame);

        // Draw detected faces
        for (const auto &face : detected_faces) {
            rectangle(frame, face, cv::Scalar(0, 255, 0));
        }


        cv::InputArray input(detected_faces);


        std::vector<std::vector<cv::Point2f> > landmarks;

        facemark->fit(frame, input, landmarks);

        //if faces are detected, draw face landmarks
        if (detected_faces.size() > 0) {
            for (unsigned int j = 0; j < detected_faces.size(); j++) {
                cv::face::drawFacemarks(frame, landmarks.at(j), cv::Scalar(0, 255, 255));
            }

        }


        imshow("Image", frame);

        if (cv::waitKey(10) == 27) {
            break;
        }

    }

    video_capture.release();

    cv::destroyAllWindows();


    return 0;
}