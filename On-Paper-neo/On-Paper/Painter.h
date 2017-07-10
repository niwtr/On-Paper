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

namespace on_paper {
    using namespace cv;
    using std::vector;
    using std::map;
    using cv::Mat;
    using cv::Point;
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
        Scalar _color;
        int _pen_size;
    public:
        int get_pen_size() const;

        void set_pen_size(int _pen_size);

    public:
        const Scalar &get_color() const;

        void set_color(const Scalar &_color);

    public:
        Point last_point;
        Mat measurement_KF;


        Mat transmatrix ; //matrix for transforming lines to original image.
    public:

        inline Mat& get_canvas_layer() { return this->canvas_layer; }
        Mat get_temp_canvas_layer();
        Painter(){}

        // M: inverted, from real to image.
        inline void with_transmatrix(const Mat &M){this->transmatrix=M;}
        // M: original, from image to real.
        void transform_canvas(const Mat& M, Size sz);
        //inline void clear_last(){ this->last_point = Point(0,0); }
        void initKF();
        void init(int rows, int cols);

        void init_canvas_of_page(int npage, int canv_height, int canv_width);

        void draw_line_simple(Point p, Scalar c);
        //must ensure the transmatrix is not empty first.
        void transform_point(Point& p);


        Point kalman_smooth(const Point &p);

        void kalman_trace(Point p, bool drawp);
        void clear_canvas(void);

        //on temp
        void paste_temp_pic(Mat pic, Point center);
        inline void draw_enlarged_rect(Rect r){ rectangle(temp_canvas, r, Scalar(0,255,255), 14); }
        void draw_finger_tips(vector<Point> fingers, int size, Scalar c);
        void text_broadcast(string str);
    };
}


#undef EUCLID_DIST
#endif //ARTEST_PAINTER_H
