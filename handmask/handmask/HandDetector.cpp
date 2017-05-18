#include "HandDetector.h"

HandDetector::HandDetector(int tar_width) :
	_tar_width(tar_width)
{
	_bgsubtractor = createBackgroundSubtractorMOG2();
	//_bgsubtractor->setVarThreshold(20);
}

void HandDetector::process(const Mat& src, Mat& dst)
{
	Mat src_nor;
	resize(src, src_nor, Size(_tar_width, _tar_width * 1.0 / src.size().width * src.size().height));

	dst = Mat::zeros(src_nor.size(), CV_8UC1);

	cvtColor(src_nor, src_nor, CV_BGR2YCrCb);

	vector<Mat> src_colors;
	split(src_nor, src_colors);

	Mat thd;
	threshold(src_colors[1], thd, 0, 255, CV_THRESH_OTSU|CV_THRESH_BINARY);

	Mat tmp;
	threashold_Y(src_nor, tmp);

	Mat img_and;
	bitwise_and(tmp, thd, img_and);

	medianBlur(img_and, img_and, 11);

	Mat img_edge;
	Canny(img_and, img_edge, 50, 250);

	vector<vector<Point> > contours;
	findContours(img_edge, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	int threshold_area = 40;
	for (int i = 0; i<contours.size(); i++) {
		double area = contourArea(contours[i]);
		if(area > threshold_area)
			drawContours(dst, contours, i, Scalar(255), 2);
	}
	
	//_bgsubtractor->apply(thd, dst, 0.01);
}

void HandDetector::process2(const Mat& src, Mat& dst)
{
	Mat src_nor;
	resize(src, src_nor, Size(_tar_width, _tar_width * 1.0 / src.size().width * src.size().height));

	dst = Mat::zeros(src_nor.size(), CV_8UC1);

	cvtColor(src_nor, src_nor, CV_BGR2YCrCb);

	Mat thd;
	threashold_YCrCb(src_nor, thd);

	int ksize = 11;
	medianBlur(thd, thd, ksize);

	Mat img_edge;
	Canny(thd, img_edge, 50, 250);

	vector<vector<Point> > contours;
	findContours(img_edge, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	int threshold_area = 40;
	for (int i = 0; i<contours.size(); i++) {
		double area = contourArea(contours[i]);
		if (area > threshold_area)
			drawContours(dst, contours, i, Scalar(255), 2);
	}
}

void HandDetector::threashold_YCrCb(const Mat& src, Mat& dst)
{
	dst = Mat::zeros(src.size(), CV_8UC1);

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			Vec3b ele = src.at<Vec3b>(i, j);
			if(ele[0] > MIN_Y && ele[1] > MIN_Cr && ele[1] < MAX_Cr && ele[2] > MIN_Cb && ele[2] < MAX_Cb)
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