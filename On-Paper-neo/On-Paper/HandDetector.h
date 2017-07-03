#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
#include "defs.h"

namespace on_paper {
	using namespace cv;
	using namespace std;

	enum ColorSpaceType {
		C_YCrCb, C_YUV, C_HSV
	};

	typedef Vec<Point, 3> Vec3p;


	class HandDetector
	{

	public:
        HandDetector(int tar_width);
        HandDetector(int tar_width, Vec3b min_ycrcb, Vec3b max_ycrcb);

		void train(const Mat& img, const Mat& mask);
		void process(const Mat& src, Mat& des);
		Point get_fingertip();
		vector<Vec3p> get_defects();

        // used for hand mask2(training before started)
        void set_thrsd(Vec3b min_ycrcb, Vec3b max_ycrcb)
        {
            _min_ycrcb = min_ycrcb;
            _max_ycrcb = max_ycrcb;
        }

	private:
		void init(int src_width);

		// two methods of hand mask
		void hand_mask(const Mat& src_std, Mat& dst);
		void hand_mask2(const Mat& src_std, Mat& dst);

		// detect and handle contours
		void hand_contours(const Mat& src, Mat& dst);
		void handle_contour(Mat& dst, const vector<Point> &contour);
		void draw_contour(Mat& dst, const vector<Point> &contour, const Scalar& color, int thickness = 1);
		int biggest_contour(vector<vector<Point> > &contours);

		// palm center and radius
		void detect_palm(const Mat& src_hand, const vector<Point>& contour);

		// tools
		void ori_correct(Mat& dst, const vector<Point> &contour);
		Point cvt_prop(Point p);

		// threshold for skin
		void threashold_C(const Mat& src, Mat& dst, int color_code);
		void threashold_YCrCb(const Mat& src, Mat& dst);
		void threashold_YUV(const Mat& src, Mat& dst);
		void threashold_Y(const Mat& src, Mat& dst);


		int _tar_width;		// 处理图像宽度
		Point _ctl_point;	// 指尖（最长手指）
		double _prop;		// 缩放比例

		vector<Vec3p> _defects;		// 凸缺陷
		Point _center;		// 掌心坐标
		int _r;				// 掌心半径

        // YCrCb threshold
        Vec3b _min_ycrcb, _max_ycrcb;
	};

}
