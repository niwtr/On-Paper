#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

enum ColorSpaceType {
	LC_YCrCb, LC_LAB, LC_HSV
};

class LcFeatureComputer
{
public:
	int dim;
	int bound;
	int veb;
	bool use_motion;
	virtual void compute(Mat & img, vector<KeyPoint> & keypts, Mat & desc) { ; }
};

template< ColorSpaceType color_type, int win_size>
class LcColorComputer : public LcFeatureComputer
{
public:
	LcColorComputer();
	void compute(Mat & img, vector<KeyPoint> & keypts, Mat & desc);
};

class HandDetector
{
public:
	void process(const Mat& src, Mat& des);

private:
	LcColorComputer<LC_HSV, 1> _cc;
};
