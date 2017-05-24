//
// Created by 牛天睿 on 17/5/24.
//
#pragma once
#ifndef ARTEST_CVUTILS_H
#define ARTEST_CVUTILS_H

#include <opencv2/core/mat.hpp>

namespace on_paper{
    using namespace std;
    using namespace cv;
    class utils {
    public:
        static void overlay_BGR(const Mat& background, const Mat& foreground, Mat& output) {
            //if(transmatrix.empty())//当transmatrix为空的时候可不要overlay。这是初始情况
            //    //TODO 尽量避免初始情况。使用更优雅的初始化函数。
            //    return;
            ////TODO 尽量减少cvtColor?
            //cvtColor(TheInputImageCopy, TheInputImageCopy, CV_BGRA2BGR);
            //Mat transf = Mat::zeros(TheInputImageCopy.size(), CV_8UC3);
            //warpPerspective(canvas, transf, this->transmatrix, TheInputImageCopy.size(), cv::INTER_NEAREST);
            cv::addWeighted(foreground, 1, background, 1, 0, output);
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
    };

}
#endif //ARTEST_CVUTILS_H