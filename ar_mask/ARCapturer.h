//
// Created by 牛天睿 on 17/5/18.
//

#ifndef ARTEST_AR_CAPTURER_H
#define ARTEST_AR_CAPTURER_H
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "aruco/aruco.h"
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include <poppler/cpp/poppler-document.h>
//#include <poppler/cpp/poppler-page.h>
//#include <poppler/cpp/poppler-page-renderer.h>
//#include <poppler/cpp/poppler-image.h>
#include <cmath>
#include <opencv/cv.hpp>

#ifdef _WIN32

#define and &&
#define or ||
#define not !

#define ROOT "C:/Users/L.Laddie/Documents/On-Paper/ar_mask/"

#else
#define ROOT "../"

#endif // _WIN32


namespace on_paper {
    /* around namespace */
    // aruco
    using aruco::Marker;
    using aruco::MarkerDetector;
    using aruco::CameraParameters;
    // cv types
    using cv::Mat;
    using cv::Point2f;
    using cv::Point;
    using cv::Scalar;
    //cv functions
    using cv::imread;
    using cv::VideoCapture;
    using cv::cvtColor;
    using cv::getPerspectiveTransform;
    using cv::perspectiveTransform;
    //std types
    using std::vector;
    using std::pair;
    //std functions
    using std::make_pair;


    constexpr const float a4_width = 0.210;
    constexpr const float a4_height = 0.297;
    constexpr const float TheMarkerSize = 0.0290;


    class ARCapturer {

    private:

        bool perform_anti_shake;

        MarkerDetector MDetector;

        vector<Marker> TheMarkers;
        vector< Marker > TheLastMarkers;
        vector<Marker> Preserved_TheMarkers;
        Mat TheInputImageCopy;
        Mat VirtualPaperImage;
        CameraParameters CamParams;

        Mat transmatrix_inv; //matrix for perspective transformation
        Mat transmatrix;

        //the image (pdf paper) to display
        Mat image;
        /*
        const vector<Point2f> pattern_marker_source = {
                Point2f(-1,1),
                Point2f(1, 1),
                Point2f(1, -1),
                Point2f(-1, -1)
        };
         */
        //这次打印的marker好像是反的，所以要两两取反。
        //上面注释掉的marker是正确的（original）。
        const vector<Point2f> pattern_marker_source = {
                Point2f(1,-1),
                Point2f(-1, -1),
                Point2f(-1, 1),
                Point2f(1, 1)
        };
        const vector<Point2f> pattern_paper_source ={
                Point2f(-a4_width/TheMarkerSize,a4_height/TheMarkerSize),
                Point2f(a4_width/TheMarkerSize,a4_height/TheMarkerSize),
                Point2f(a4_width/TheMarkerSize, -a4_height/TheMarkerSize),
                Point2f(-a4_width/TheMarkerSize, -a4_height/TheMarkerSize)
        };

        vector<vector<Point2f>> shifted_pattern_paper_source;


        //上负下正，左负右正
        const vector<pair<float, float>> shifts = {
                pair<float, float>((a4_width-TheMarkerSize)/2/a4_width, (a4_height-TheMarkerSize)/2/a4_height),
                pair<float, float>(0, 0.16667),
                pair<float, float>(0, -0.16667),
                pair<float, float>((a4_width-TheMarkerSize)/2/a4_width, -(a4_height-TheMarkerSize)/2/a4_height),
        };
        vector<Point2f> original_image_pattern;

    public:
        const Mat& get_transmatrix(){return transmatrix;}
        const Mat& get_transmatrix_inv(){return transmatrix_inv;}
        void get_input_image(Mat& m){ m.copyTo(this->TheInputImageCopy); }
        const Mat& get_image(){ return this->image;}
        Mat get_virtual_paper_layer(){return this->VirtualPaperImage;}
        unsigned long process();
        void init(CameraParameters cp);
        void toggle_anti_shake(){this->perform_anti_shake = not this->perform_anti_shake;}


    private:

        template<typename vecpf>
        float euclid_dist(const vecpf& v1, const vecpf& v2);

        void white_transparent(const Mat & src, Mat& dst);

        vector<Point2f>vector_avg2 (const vector<Point2f>& src1, const vector<Point2f>& src2);
        void map_markers(void);//returns marker num.
        cv::Mat resize(const cv::Mat &in,int width);
        void anti_shake(void);
        void fill_markers(void);
    };

}

#endif //ARTEST_AR_CAPTURER_H
