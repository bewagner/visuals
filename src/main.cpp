#include "detector/Detector.h"
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/cudaoptflow.hpp>
#include <opencv4/opencv2/cudaarithm.hpp>

using namespace std;
using namespace cv;
using namespace cv::cuda;

inline bool isFlowCorrect(Point2f u) {
    return !cvIsNaN(u.x) && !cvIsNaN(u.y) && fabs(u.x) < 1e9 && fabs(u.y) < 1e9;
}

static Vec3b computeColor(float fx, float fy) {
    static bool first = true;
    // relative lengths of color transitions:
    // these are chosen based on perceptual similarity
    // (e.g. one can distinguish more shades between red and yellow
    //  than between yellow and green)
    const int RY = 15;
    const int YG = 6;
    const int GC = 4;
    const int CB = 11;
    const int BM = 13;
    const int MR = 6;
    const int NCOLS = RY + YG + GC + CB + BM + MR;
    static Vec3i colorWheel[NCOLS];
    if (first) {
        int k = 0;
        for (int i = 0; i < RY; ++i, ++k)
            colorWheel[k] = Vec3i(255, 255 * i / RY, 0);
        for (int i = 0; i < YG; ++i, ++k)
            colorWheel[k] = Vec3i(255 - 255 * i / YG, 255, 0);
        for (int i = 0; i < GC; ++i, ++k)
            colorWheel[k] = Vec3i(0, 255, 255 * i / GC);
        for (int i = 0; i < CB; ++i, ++k)
            colorWheel[k] = Vec3i(0, 255 - 255 * i / CB, 255);
        for (int i = 0; i < BM; ++i, ++k)
            colorWheel[k] = Vec3i(255 * i / BM, 0, 255);
        for (int i = 0; i < MR; ++i, ++k)
            colorWheel[k] = Vec3i(255, 0, 255 - 255 * i / MR);
        first = false;
    }
    const float rad = sqrt(fx * fx + fy * fy);
    const float a = atan2(-fy, -fx) / (float) CV_PI;
    const float fk = (a + 1.0f) / 2.0f * (NCOLS - 1);
    const int k0 = static_cast<int>(fk);
    const int k1 = (k0 + 1) % NCOLS;
    const float f = fk - k0;
    Vec3b pix;
    for (int b = 0; b < 3; b++) {
        const float col0 = colorWheel[k0][b] / 255.0f;
        const float col1 = colorWheel[k1][b] / 255.0f;
        float col = (1 - f) * col0 + f * col1;
        if (rad <= 1)
            col = 1 - rad * (1 - col); // increase saturation with radius
        else
            col *= .75; // out of range
        pix[2 - b] = static_cast<uchar>(255.0 * col);
    }
    return pix;
}

static void drawOpticalFlow(const Mat_<float> &flowx, const Mat_<float> &flowy, Mat &dst, float maxmotion = -1) {
    dst.create(flowx.size(), CV_8UC3);
    dst.setTo(Scalar::all(0));
    // determine motion range:
    float maxrad = maxmotion;
    if (maxmotion <= 0) {
        maxrad = 1;
        for (int y = 0; y < flowx.rows; ++y) {
            for (int x = 0; x < flowx.cols; ++x) {
                Point2f u(flowx(y, x), flowy(y, x));
                if (!isFlowCorrect(u))
                    continue;
                maxrad = max(maxrad, sqrt(u.x * u.x + u.y * u.y));
            }
        }
    }
    for (int y = 0; y < flowx.rows; ++y) {
        for (int x = 0; x < flowx.cols; ++x) {
            Point2f u(flowx(y, x), flowy(y, x));
            if (isFlowCorrect(u))
                dst.at<Vec3b>(y, x) = computeColor(u.x / maxrad, u.y / maxrad);
        }
    }
}

static void showFlow(const char *name, const GpuMat &d_flow) {
    GpuMat planes[2];
    cv::cuda::split(d_flow, planes);

    Mat flowx(planes[0]);
    Mat flowy(planes[1]);
    Mat out;
    drawOpticalFlow(flowx, flowy, out, 10);
    imshow(name, out);

}


int main(int argc, char **argv) {
    cv::VideoCapture video_capture;
    if (!video_capture.open(0)) {
        return 0;
    }


    cv::Mat h_frame, h_frame_grayscale, h_flow;
    cv::cuda::GpuMat d_frame_grayscale;
    cv::cuda::GpuMat d_frame_grayscale_f, d_old_frame_grayscale_f;
    cv::cuda::GpuMat d_flow_f;

    auto optical_flow = cv::cuda::FarnebackOpticalFlow::create();
    unsigned long i = 0;
    while (true) {

        video_capture >> h_frame;
        cv::cvtColor(h_frame, h_frame_grayscale, cv::COLOR_BGR2GRAY);
        d_frame_grayscale.upload(h_frame_grayscale);

        d_frame_grayscale.convertTo(d_frame_grayscale_f, CV_32F, 1.0f / 255.0f);

        if (i > 0) {
            optical_flow->calc(d_frame_grayscale_f, d_old_frame_grayscale_f, d_flow_f);
        }
        d_old_frame_grayscale_f = d_frame_grayscale_f.clone();

        if (i > 0) {
            showFlow("Name", d_flow_f);
            if (cv::waitKey(10) == 27) {
                break;
            }
        }


//        if (i > 0) {
//            d_flow_f.download(h_flow);
//            imshow("Image", h_flow);

//        }
        i++;
    }
    video_capture.release();
    cv::destroyAllWindows();

    return 0;
}