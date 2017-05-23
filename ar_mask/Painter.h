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

        Point last_point;
    public:

        Mat &get_canvas() { return this->canvas; }

        Painter(){}

        void init(int rows, int cols) {
            this->canv_height = rows;
            this->canv_width = cols;
            this->canvas = Mat::zeros(rows, cols, CV_8UC3);
        }
        void draw_point(Point p, Scalar c){
            if(p.x<10 or p.y<10)
                return;
            if(last_point.x<10 or last_point.y<10){
                last_point = p;
                return;
            }
            //circle(this->canvas, p, 3, c, 6);

            line(canvas, last_point, p, c, 2, LINE_AA);
            last_point = p;

        }

        void draw_points(vector<Point> &v, Scalar c);

        void clear_canvas(void);
    };
}


#endif //ARTEST_PAINTER_H
