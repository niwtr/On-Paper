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

#define EUCLID_DIST(A,B) (sqrt(fabs(pow(A.x - B.x, 2) + pow(A.y - B.y, 2))))



namespace on_paper {
    using namespace cv;
    using std::vector;

    class Painter {
    private:
        Mat canvas;
        int canv_height;
        int canv_width;
        cv::KalmanFilter KF;
        Point last_point;

        Mat state_KF;
        Mat measurement_KF;
        Mat process_noise_KF;


    public:

        Mat &get_canvas() { return this->canvas; }

        Painter(){}

        void init(int rows, int cols) {
            this->canv_height = rows;
            this->canv_width = cols;
            this->canvas = Mat::zeros(rows, cols, CV_8UC3);
            /* init kalman filter */

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
        void draw_line_simple(Point p, Scalar c){
            if(p.x<10 or p.y<10)
                return;
            if(last_point.x<10 or last_point.y<10){
                last_point = p;
                return;
            }



            line(canvas, last_point, p, c, 2, LINE_AA);
            last_point = p;

        }



        void draw_line_kalman(Point p, Scalar c){

            if(p.x<10 or p.y<10)
                return;
            if(last_point.x<10 or last_point.y<10){
                last_point = p;
                return;
            }

            //  predict, to update the internal statePre variable
            Mat prediction = KF.predict();
            Point predictPt(prediction.at<int>(0),prediction.at<int>(1));

            //get point and correct
            measurement_KF.at<int>(0) = p.x;
            measurement_KF.at<int>(1) = p.y;


            //Point measPt(measurement_KF(0),measurement_KF(1));
            Point measPt(measurement_KF);


            Mat estimated = KF.correct(measurement_KF);
            Point statePt(estimated.at<int>(0),estimated.at<int>(1));

            line(canvas, last_point, statePt, c, 2, LINE_AA);
            last_point =statePt;
        }

        void draw_points(vector<Point> &v, Scalar c);

        void clear_canvas(void);
    };
}


#endif //ARTEST_PAINTER_H
