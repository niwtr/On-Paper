#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

enum ColorSpaceType {
	C_YCrCb, C_YUV, C_HSV
};

typedef Vec<Point, 3> Vec3p;

#define MIN_Cr 113
#define MAX_Cr 173
#define MIN_Cb 77
#define MAX_Cb 127
#define MIN_Y 30

#define THRESHOLD_AREA 20
#define MAX_CONTOURS_SIZE 5
#define THRESHOLD_ANGLE 80
#define BLUR_KSIZE 7
#define PI 3.14

class HandDetector
{
public:
	HandDetector(int tar_width);

	void train(const Mat& img, const Mat& mask);
	void process(const Mat& src, Mat& des);
	Point get_fingertip();

	// two methods of hand mask
	void hand_mask(const Mat& src_std, Mat& dst);
	void hand_mask2(const Mat& src_std, Mat& dst);

	// detect and handle contours
	void hand_contours(const Mat& src, Mat& dst);
	void handle_contour(Mat& dst, const vector<Point> &contour, const Point center, const int r);
	void draw_contour(Mat& dst, const vector<Point> &contour, const Scalar& color, int thickness = 1);
	int biggest_contour(vector<vector<Point> > &contours);

	// palm center and radius
	void detect_palm(const Mat& src_hand, const vector<Point>& contour, Point& center, int& r);

	void ori_correct(Mat& dst, const vector<Point> &contour);
	// tools
	void drawLine(cv::Mat &image, double theta, double rho, cv::Scalar color);
	float distance_P2P(Point a, Point b);
	float get_angle(Point s, Point f, Point e);

	// threshold for skin
	void threashold_C(const Mat& src, Mat& dst, int color_code);
	void threashold_YCrCb(const Mat& src, Mat& dst);
	void threashold_YUV(const Mat& src, Mat& dst);

	void threashold_Y(const Mat& src, Mat& dst);

    static void init_threshold(int event, int x, int y, int flags, void* vhd);

    void update_ycrcb(Vec3b p_ycrcb);
    const Mat* _last_img;

private:
	int _tar_width;
	Point _ctl_point;
	double _prop;

    Vec3b min_ycrcb = Vec3b(255, 255, 255);
    Vec3b max_ycrcb = Vec3b(0, 0, 0);
};
