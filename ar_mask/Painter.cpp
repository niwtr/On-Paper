//
// Created by 牛天睿 on 17/5/22.
//


#include "Painter.h"

//void on_paper::Painter::draw_points(std::vector<cv::Point> &v, Scalar c) {
//    for(auto& p : v) {
//        circle(this->canvas, p, 2, c, 2);
//    }
//}

void on_paper::Painter::clear_canvas(void)
{
    Mat& dst = this->canvas;
    for (int y = 0; y < dst.rows; ++y)
        for (int x = 0; x < dst.cols; ++x)
        {
            cv::Vec3b & pixel = dst.at<cv::Vec3b>(y, x);
            pixel[0]=0;
            pixel[1]=0;
            pixel[2]=0;
        }
};