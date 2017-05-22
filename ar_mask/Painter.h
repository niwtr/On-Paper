//
// Created by 牛天睿 on 17/5/22.
//

#ifndef ARTEST_PAINTER_H
#define ARTEST_PAINTER_H


#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <iostream>

namespace on_paper {
    using namespace cv;
    using std::vector;

    class Painter {
    private:
        Mat canvas;
        int canv_height;
        int canv_width;

    public:

        Mat &get_canvas() { return this->canvas; }

        Painter(){}

        void init(int rows, int cols) {
            this->canv_height = rows;
            this->canv_width = cols;
            this->canvas = Mat::zeros(rows, cols, CV_8UC3);
            //cvtColor(canvas, canvas, CV_BGR2BGRA);
        }
        void draw_point(Point p, Scalar c){
            circle(this->canvas, p, 3, c, 6);
        }

        void draw_points(vector<Point> &v, Scalar c);

        void clear_canvas(void);
    };
}


#endif //ARTEST_PAINTER_H
