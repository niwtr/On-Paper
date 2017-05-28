//
// Created by 牛天睿 on 17/5/22.
//

#ifndef ARTEST_PAINTER_H
#define ARTEST_PAINTER_H


#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <iostream>
#include <opencv/cv.hpp>
#include <vector>
#include <opencv2/highgui.hpp>
#include "cvutils.h"
#include <map>

#define EUCLID_DIST(A,B) (sqrt(fabs(pow(A.x - B.x, 2) + pow(A.y - B.y, 2))))

#ifdef _WIN32

#define and &&
#define or ||
#define not !

#endif // _WIN32

namespace on_paper {
    using namespace cv;
    using std::vector;
    using std::map;

    class Painter {
    private:
        //canvas是当前工作中的画布。
        //canvas_container是画布的容器。
        //canvas_layer是映射到真实世界的canvas
        //temp_canvas是单帧canvas
        //temp_canvas_layer是单帧canvas的投射。
        Mat __empty;
        Mat temp_canvas;
        Mat temp_canvas_layer;
        Mat canvas;
        map<int, Mat> canvas_container;
        Mat canvas_layer;
        int canv_height;
        int canv_width;
        cv::KalmanFilter KF;
    public:
        Point last_point;
        Mat measurement_KF;


        Mat transmatrix ; //matrix for transforming lines to original image.
    public:

        Mat& get_canvas_layer() { return this->canvas_layer; }
        Mat get_temp_canvas_layer(){
            auto _layer = this->temp_canvas_layer;
            this->temp_canvas = __empty.clone();
            //this->temp_canvas_layer = __empty.clone();
            return _layer;
        }
        Painter(){}


        // M: inverted, from real to image.
        void with_transmatrix(const Mat &M){this->transmatrix=M;}
        // M: original, from image to real.
        void transform_canvas(const Mat& M, Size sz){
            if(M.empty())
                return;
            warpPerspective(canvas, canvas_layer, M, sz, INTER_NEAREST);
            warpPerspective(temp_canvas, temp_canvas_layer, M, sz, INTER_NEAREST);
        }
        void clear_last(){
            this->last_point = Point(0,0);
        }
        void initKF(){

            const int stat_num = 4;//是什么？
            const int measure_num = 2;//是什么？
            KF.init(stat_num, measure_num, 0);

            KF.transitionMatrix = (Mat_<float>(4, 4) << 1,0,1,0,   0,1,0,1,  0,0,1,0,  0,0,0,1);
            Mat_<float> measurement(2,1); measurement.setTo(Scalar(0));
            measurement_KF=measurement;

            // init...
            KF.statePre.at<float>(0) = 0;
            KF.statePre.at<float>(1) = 0;
            KF.statePre.at<float>(2) = 0;
            KF.statePre.at<float>(3) = 0;
            setIdentity(KF.measurementMatrix);
            setIdentity(KF.processNoiseCov, Scalar::all(1e-4));
            setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));
            setIdentity(KF.errorCovPost, Scalar::all(.1));
        }
        void init(int rows, int cols) {
            this->canv_height = rows;
            this->canv_width = cols;
            this->canvas = Mat::zeros(rows, cols, CV_8UC3);
            /* init kalman filter */
            this->__empty = Mat::zeros(rows, cols, CV_8UC3);
            this->temp_canvas = __empty.clone();
            initKF();
        }

        void init_canvas_of_page(int npage){
            auto it = canvas_container.find(npage);
            if(it == canvas_container.end()) {//init new
                Mat newcanvas = Mat::zeros(canv_height, canv_width, CV_8UC3);
                this->canvas = newcanvas;
                this->canvas_container.insert(make_pair<int, Mat>(move(npage), move(newcanvas)));
            } else
                this->canvas = it->second;
        }


        void draw_line_simple(Point p, Scalar c){
            if(p.x<10 or p.y<10 )
                return;
            if(last_point.x<10 or last_point.y<10){
                last_point = p;
                return;
            }

            line(canvas, last_point, p, c, 2, LINE_AA);
            last_point = p;

        }

        //must ensure the transmatrix is not empty first.
        void transform_point(Point& p){
            vector<Point2f> mreal = {p};
            vector<Point2f> mimage;
            perspectiveTransform(mreal,mimage, this->transmatrix );
            p = mimage[0];
        }

        void draw_enlarged_rect(Rect r){
            rectangle(temp_canvas, r, Scalar(0,255,255), 7);
        }

        Point kalman_smooth(const Point &p){

            //  predict, to update the internal statePre variable
            Mat prediction = KF.predict();
            Point predictPt(prediction.at<int>(0),prediction.at<int>(1));

            //get point and correct
            measurement_KF.at<int>(0) = p.x;
            measurement_KF.at<int>(1) = p.y;

            Point measPt(measurement_KF);
            Mat estimated = KF.correct(measurement_KF);
            Point statePt(estimated.at<int>(0),estimated.at<int>(1));
            return statePt;
        }

        void kalman_trace(Point p, int sz, Scalar c, bool drawp) {
            if(transmatrix.empty())return;
            transform_point(p);

            if(p.x<0 or p.y<0 or p.x > canv_width or p.y > canv_height)
                return;
            if(last_point.x<10 or last_point.y<10
                    or
               last_point.x > canv_width or last_point.y > canv_height){
                last_point = p;
                return;
            }
            Point statePt = kalman_smooth(p);
            if(drawp)
                line(canvas, last_point, statePt, c, sz, LINE_AA);
            last_point =statePt;
        }


        void clear_canvas(void);

    };
}


#undef EUCLID_DIST
#endif //ARTEST_PAINTER_H
