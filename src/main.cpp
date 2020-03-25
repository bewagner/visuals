#include <stdio.h>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/face.hpp>

using namespace cv;
using namespace cv::face;

int main(int argc, char **argv) {

    CascadeClassifier faceDetector("haarcascade_frontalface_alt2.xml");

    // Create an instance of Facemark
//    Ptr<Facemark> facemark = FacemarkLBF::create();

    // Load landmark detector
//    facemark->loadModel("lbfmodel.yaml");

    VideoCapture video_capture;
    if (!video_capture.open(0)) {
        return 0;
    }
    Mat frame, gray;
    while (true) {

        std::vector<Rect> faces;

        video_capture >> frame;

        if (frame.empty()) {
            break;
        }

        cvtColor(frame, gray, COLOR_BGR2GRAY);

        // Detect faces
        try {
            faceDetector.detectMultiScale(gray, faces);
        } catch(const cv::Exception & e){
            std::cout << e.what() << std::endl;
            continue;
        }


        // Draw detected faces
        if (!faces.empty()) {
            for (const auto &face : faces) {
                rectangle(frame, face, Scalar(0, 255, 0));
            }
        }


        std::cout << faces.size() << std::endl;

        imshow("Image", frame);

        if (waitKey(10) == 27) {
            break;
        }

    }

    video_capture.release();

    destroyAllWindows();


    return 0;
}