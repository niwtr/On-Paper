#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

enum ColorSpaceType {
	LC_YCrCb, LC_LAB, LC_HSV
};

#define MIN_Cr 135
#define MAX_Cr 180
#define MIN_Cb 85
#define MAX_Cb 135
#define MIN_Y 80

class HandDetector
{
public:
	HandDetector(int tar_width);
	void process(const Mat& src, Mat& des);

	void threashold_YCrCb(const Mat& src, Mat& dst, Mat& thd);

private:
	int _tar_width;
	Ptr<BackgroundSubtractorMOG2> _bgsubtractor;
};
