//
// Created by 牛天睿 on 17/5/24.
//
#pragma once
#ifndef ARTEST_CVUTILS_H
#define ARTEST_CVUTILS_H

#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <sys/time.h>
namespace on_paper{
    using namespace std;
    using namespace cv;
    class utils {
    public:
        constexpr const static float PI = 3.14;

        static long curtime_msec(void){
            struct timeval tv;
            gettimeofday(&tv,NULL);
            return tv.tv_sec * 1000 + tv.tv_usec / 1000;
        };


        static void drawLine(cv::Mat &image, double theta, double rho, cv::Scalar color){
                if (theta < PI / 4. || theta > 3.*PI / 4.)// ~vertical line
                {
                    Point pt1(rho / cos(theta), 0);
                    Point pt2((rho - image.rows * sin(theta)) / cos(theta), image.rows);
                    line(image, pt1, pt2, Scalar(255), 1);
                }
                else
                {
                    Point pt1(0, rho / sin(theta));
                    Point pt2(image.cols, (rho - image.cols * cos(theta)) / sin(theta));
                    line(image, pt1, pt2, color, 1);
                }
        }

        // 两点距离
        static float distance_P2P(cv::Point a, cv::Point b) {
            float d = sqrt(fabs(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)));
            return d;
        }

// 获得三点角度(弧度制)
        static float get_angle(cv::Point s, cv::Point f, cv::Point e) {
            float l1 = distance_P2P(f, s);
            float l2 = distance_P2P(f, e);
            float dot = (s.x - f.x)*(e.x - f.x) + (s.y - f.y)*(e.y - f.y);
            float angle = acos(dot / (l1*l2));
            angle = angle * 180 / PI;
            return angle;
        }


        static void overlay_BGR(const Mat& background, const Mat& foreground, Mat& output) {
            cv::addWeighted(foreground, 1, background, 1, 0, output);
        }

        static void white_transparent(const Mat & src, Mat& dst){
            cv::cvtColor(src, dst, CV_BGR2BGRA);
            // find all white pixel and set alpha value to zero:
            for (int y = 0; y < dst.rows; ++y)
                for (int x = 0; x < dst.cols; ++x)
                {
                    cv::Vec4b & pixel = dst.at<cv::Vec4b>(y, x);
                    // if pixel is white
                    if (pixel[0] > 180 && pixel[1] > 180 && pixel[2] > 180)
                    {
                        // set alpha to zero:
                        pixel[3] = 0;
                    }
                }
        }

        static void overlay_BGRA(const Mat &background, const Mat &foreground,
                     Mat &output, Point2i location) {
            background.copyTo(output);
            // start at the row indicated by location, or at row 0 if location.y is negative.
            for (int y = std::max(location.y, 0); y < background.rows; ++y) {
                int fY = y - location.y; // because of the translation

                // we are done of we have processed all rows of the foreground image.
                if (fY >= foreground.rows)
                    break;
                // start at the column indicated by location,
                // or at column 0 if location.x is negative.
                for (int x = std::max(location.x, 0); x < background.cols; ++x) {
                    int fX = x - location.x; // because of the translation.

                    // we are done with this row if the column is outside of the foreground image.
                    if (fX >= foreground.cols)
                        break;

                    // determine the opacity of the foregrond pixel, using its fourth (alpha) channel.
                    double opacity =
                            ((double) foreground.data[fY * foreground.step + fX * foreground.channels() + 3])

                            / 255.;
                    // and now combine the background and foreground pixel, using the opacity,
                    // but only if opacity > 0.
                    for (int c = 0; opacity > 0 && c < output.channels(); ++c) {
                        unsigned char foregroundPx =
                                foreground.data[fY * foreground.step + fX * foreground.channels() + c];
                        unsigned char backgroundPx =
                                background.data[y * background.step + x * background.channels() + c];
                        output.data[y * output.step + output.channels() * x + c] =
                                backgroundPx * (1. - opacity) + foregroundPx * opacity;
                    }
                }
            }
        }
        static string intostr(int i)
        {
            string str;
            stringstream ss;
            ss<<i;
            ss>>str;
            return str;
        }

        static string into_name(int page,int Num)
        {
            string name;
            int i,zeronum=Num-1;
            i=page;
            while(i=i/10)
            {
                zeronum--;
            }
            for(int j=0;j<zeronum;j++)
                name+="0";
            name=name+intostr(page);
            return name;
        }
    };

}
#endif //ARTEST_CVUTILS_H