#include "HandDetector.h"

HandDetector::HandDetector(int tar_width) :
	_tar_width(tar_width)
{
	_bgsubtractor = createBackgroundSubtractorMOG2();
	//_bgsubtractor->setVarThreshold(20);
}

void HandDetector::process(const Mat& src, Mat& dst)
{
	if (!dst.data)
		dst.create(src.size(), CV_8UC1);

	Mat tmp;
	resize(src, tmp, Size(_tar_width, _tar_width * 1.0 / dst.size().width * dst.size().height));

	Mat tmp_color;
	cvtColor(tmp, tmp_color, CV_BGR2YCrCb);

	vector<Mat> tmp_colors;
	split(tmp_color, tmp_colors);

	Mat thd;
	threshold(tmp_colors[1], thd, 0, 255, CV_THRESH_OTSU|CV_THRESH_BINARY);

	medianBlur(thd, thd, 11);

	//threashold_YCrCb(tmp_color, dst, thd);

	
	dst = thd;
	//_bgsubtractor->apply(thd, dst, 0.01);

	//vector<vector<Point> > contours;
	//findContours(thd, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	//dst.create(thd.rows, thd.cols, CV_8UC1);
	//for (int i = 0; i<contours.size(); i++) {
	//	drawContours(dst, contours, i, Scalar(0), 3);
	//}
}

void HandDetector::threashold_YCrCb(const Mat& src, Mat& dst, Mat& thd)
{
	dst.create(src.size(), CV_8UC1);

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