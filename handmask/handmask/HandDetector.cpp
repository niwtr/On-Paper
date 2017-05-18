#include "HandDetector.h"

HandDetector::HandDetector(int tar_width) :
	_tar_width(tar_width)
{
	_bgsubtractor = createBackgroundSubtractorMOG2();
	//_bgsubtractor->setVarThreshold(20);
}

void HandDetector::train(const Mat& img, const Mat& mask)
{
	Mat img_std, mask_std;
	resize(img, img_std, Size(_tar_width, _tar_width * 1.0 / img.size().width * img.size().height));
	resize(mask, mask_std, Size(_tar_width, _tar_width * 1.0 / img.size().width * img.size().height));

	cvtColor(img_std, img_std, CV_BGR2Lab);
	if (mask.channels() == 3)
		cvtColor(mask_std, mask_std, CV_BGR2GRAY);

	Vec3i avg_color(0, 0, 0);
	int cnt = 0;
	for (int i = 0; i < img_std.rows; i++)
	{
		for (int j = 0; j < img_std.cols; j++)
		{
			if (mask_std.at<uchar>(i, j) == 255)
			{
				Vec3b cur = img_std.at<Vec3b>(i, j);
				avg_color += cur;
				cnt++;
			}
		}
	}

	cout << avg_color / cnt << endl;
}

// used otsu + YCrCb filter
void HandDetector::process(const Mat& src, Mat& dst)
{
	Mat src_std;
	resize(src, src_std, Size(_tar_width, _tar_width * 1.0 / src.size().width * src.size().height));

	dst = Mat::zeros(src_std.size(), CV_8UC1);

	cvtColor(src_std, src_std, CV_BGR2YCrCb);

	vector<Mat> src_colors;
	split(src_std, src_colors);

	Mat thd;
	threshold(src_colors[1], thd, 0, 255, CV_THRESH_OTSU|CV_THRESH_BINARY);

	Mat tmp;
	threashold_Y(src_std, tmp);

	Mat img_and;
	bitwise_and(tmp, thd, img_and);

	medianBlur(img_and, img_and, 7);

	Mat img_edge;
	Canny(img_and, img_edge, 50, 250);

	hand_contours(img_edge, dst);
	
	//_bgsubtractor->apply(thd, dst, 0.01);
}

// simple color filter
void HandDetector::process2(const Mat& src, Mat& dst)
{
	Mat src_std;
	resize(src, src_std, Size(_tar_width, _tar_width * 1.0 / src.size().width * src.size().height));

	dst = Mat::zeros(src_std.size(), CV_8UC1);

	cvtColor(src_std, src_std, CV_BGR2YCrCb);

	Mat thd;
	threashold_YCrCb(src_std, thd);

	int ksize = 7;
	medianBlur(thd, thd, ksize);

	Mat img_edge;
	Canny(thd, img_edge, 50, 250);

	hand_contours(img_edge, dst);
}

void HandDetector::threashold_YCrCb(const Mat& src, Mat& dst)
{
	threashold_C(src, dst, C_YCrCb);
}

void HandDetector::threashold_YUV(const Mat& src, Mat& dst)
{
	threashold_C(src, dst, C_YUV);
}

// color filter with different kind of color space type
void HandDetector::threashold_C(const Mat& src, Mat& dst, int color_code)
{
	dst = Mat::zeros(src.size(), CV_8UC1);

	Vec3b min_v, max_v;
	if (color_code == C_YCrCb)
	{
		min_v = Vec3b(MIN_Y, MIN_Cr, MIN_Cb);
		max_v = Vec3b(255, MAX_Cr, MAX_Cb);
	}
	else if (color_code == C_YUV)
	{
		min_v = Vec3b(MIN_Y2, MIN_U, MIN_V);
		max_v = Vec3b(MAX_Y2, MAX_U, MAX_V);
	}

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			Vec3b ele = src.at<Vec3b>(i, j);
			if(ele[0] > min_v[0] && ele[0] < max_v[0] && ele[1] > min_v[1] && ele[1] < max_v[1] && ele[2] > min_v[2] && ele[2] < max_v[2])
			{
				dst.at<uchar>(i, j) = 255;
			}
			else
			{
				dst.at<uchar>(i, j) = 0;
			}
		}
	}
}

// color filter only in YCrCb space of Y
void HandDetector::threashold_Y(const Mat& src, Mat& dst)
{
	dst = Mat::zeros(src.size(), CV_8UC1);

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			Vec3b ele = src.at<Vec3b>(i, j);
			if (ele[0] > MIN_Y)
			{
				dst.at<uchar>(i, j) = 255;
			}
			else
			{
				dst.at<uchar>(i, j) = 0;
			}
		}
	}
}

void HandDetector::hand_contours(const Mat& src, Mat& dst)
{
	vector<vector<Point> > contours;
	findContours(src, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	vector<vector<Point> >hull(contours.size());
	vector<vector<int>> hullsI(contours.size());
	vector<vector<Vec4i>> defects(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		convexHull(Mat(contours[i]), hull[i], false);
		convexHull(Mat(contours[i]), hullsI[i], false);
		convexityDefects(Mat(contours[i]), hullsI[i], defects[i]);
	}


	for (int i = 0; i<contours.size(); i++) {
		double area = contourArea(contours[i]);
		if (area > THRESHOLD_AREA)
		{
			Scalar color = Scalar(255);
			drawContours(dst, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());
			drawContours(dst, hull, i, color, 1, 8, vector<Vec4i>(), 0, Point());

			//// draw defects
			//size_t count = contours[i].size();
			//if (count < 300)
			//	continue;

			//vector<Vec4i>::iterator d = defects[i].begin();

			//while (d != defects[i].end()) {
			//	Vec4i& v = (*d);
			//	int startidx = v[0];
			//	Point ptStart(contours[i][startidx]); // point of the contour where the defect begins
			//	int endidx = v[1];
			//	Point ptEnd(contours[i][endidx]); // point of the contour where the defect ends
			//	int faridx = v[2];
			//	Point ptFar(contours[i][faridx]);// the farthest from the convex hull point within the defect
			//	int depth = v[3]; // distance between the farthest point and the convex hull

			//	cout << depth << endl;

			//	if (depth > 20 && depth < 80)
			//	{
			//		line(dst, ptStart, ptFar, CV_RGB(0, 255, 0), 2);
			//		line(dst, ptEnd, ptFar, CV_RGB(0, 255, 0), 2);
			//		circle(dst, ptStart, 4, Scalar(255, 0, 100), 2);
			//		circle(dst, ptEnd, 4, Scalar(255, 0, 100), 2);
			//		circle(dst, ptFar, 4, Scalar(100, 0, 255), 2);
			//	}
			//	d++;
			//}
		}
	}
}