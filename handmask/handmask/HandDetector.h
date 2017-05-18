#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

enum ColorSpaceType {
	LC_YCrCb, LC_LAB, LC_HSV
};

#define MIN_Cr 120
#define MAX_Cr 180
#define MIN_Cb 80
#define MAX_Cb 135
#define MIN_Y 30

class HandDetector
{
public:
	HandDetector(int tar_width);
	void process(const Mat& src, Mat& des);
	void process2(const Mat& src, Mat& des);

	void threashold_YCrCb(const Mat& src, Mat& dst);
	void threashold_Y(const Mat& src, Mat& dst);

private:
	int _tar_width;
	Ptr<BackgroundSubtractorMOG2> _bgsubtractor;
};
