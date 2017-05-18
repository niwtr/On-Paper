#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

enum ColorSpaceType {
	C_YCrCb, C_YUV, C_HSV
};

#define MIN_Cr 113
#define MAX_Cr 173
#define MIN_Cb 77
#define MAX_Cb 127
#define MIN_Y 30

#define MIN_Y2 65
#define MAX_Y2 170
#define MIN_U 85
#define MAX_U 140
#define MIN_V 85
#define MAX_V 160

#define THRESHOLD_AREA 20

class HandDetector
{
public:
	HandDetector(int tar_width);

	void train(const Mat& img, const Mat& mask);

	void process(const Mat& src, Mat& des);
	void process2(const Mat& src, Mat& des);

	void hand_contours(const Mat& src, Mat& dst);

	void threashold_C(const Mat& src, Mat& dst, int color_code);
	void threashold_YCrCb(const Mat& src, Mat& dst);
	void threashold_YUV(const Mat& src, Mat& dst);

	void threashold_Y(const Mat& src, Mat& dst);

private:
	int _tar_width;
	Ptr<BackgroundSubtractorMOG2> _bgsubtractor;
};
