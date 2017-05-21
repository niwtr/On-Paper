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

	dst = Mat::zeros(src_std.size(), CV_8UC3);

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
	
	hand_contours(img_and, src_std);
	dst = src_std;
	
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

	hand_contours(thd, dst);
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
	else
		return;

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


// find the max-area of contour
int HandDetector::biggest_contour(vector<vector<Point> > &contours)
{
	int idx = 0;
	int area = 0;
	for (int i = 0; i < contours.size(); i++) {
		double tmp_area = contourArea(contours[i]);
		if (tmp_area > area) {
			area = tmp_area;
			idx = i;
		}
	}
	return idx;
}

// draw single contour
void HandDetector::draw_contour(Mat& dst, const vector<Point> &contour, const Scalar& color, int thickness)
{
	vector<vector<Point> > tmp_points;
	tmp_points.push_back(contour);
	drawContours(dst, tmp_points, 0, color, 1);
}

void HandDetector::drawLine(cv::Mat &image, double theta, double rho, cv::Scalar color)
{
	if (theta < PI / 4. || theta > 3.*PI / 4.)// ~vertical line
	{
		cv::Point pt1(rho / cos(theta), 0);
		cv::Point pt2((rho - image.rows * sin(theta)) / cos(theta), image.rows);
		cv::line(image, pt1, pt2, cv::Scalar(255), 1);
	}
	else
	{
		cv::Point pt1(0, rho / sin(theta));
		cv::Point pt2(image.cols, (rho - image.cols * cos(theta)) / sin(theta));
		cv::line(image, pt1, pt2, color, 1);
	}
}

// 矫正手部方向
void HandDetector::ori_correct(Mat& dst, const vector<Point> &contour)
{
	Vec4f li;
	fitLine(contour, li, CV_DIST_HUBER, 0, 0.01, 0.01);
	double cos_theta = li[0];
	double sin_theta = li[1];
	double x0 = li[2], y0 = li[3];

	double phi = atan2(sin_theta, cos_theta) + PI / 2.0;
	double rho = y0 * cos_theta - x0 * sin_theta;

	drawLine(dst, phi, rho, cv::Scalar(0));
}

// 检测手掌
void HandDetector::detect_palm(const Mat& src_hand, const vector<Point>& contour, Mat& dst_palm)
{
	// 距离变换
	Mat dist;
	distanceTransform(src_hand, dist, DIST_L2, 3);

	// 检测最小矩形，缩小检测范围
	Rect rec = boundingRect(contour);

	int temp = 0, R = 0, cx = 0, cy = 0, flag;
	for (int i = rec.y; i < rec.y + rec.height; i++)
	{
		for (int j = rec.x; j < rec.x + rec.width; j++)
		{
			// 检测是否在轮廓内，影响实时性
			//flag = pointPolygonTest(contour, Point2f(j, i), 0);	
			//if (flag > 0)
			{
				temp = (int)dist.at<float>(i, j);
				if (temp > R)
				{
					R = temp;
					cy = i;
					cx = j;
				}
			}
		}
	}

	src_hand.copyTo(dst_palm);
	cvtColor(dst_palm, dst_palm, CV_GRAY2BGR);
	circle(dst_palm, Point(cx, cy), R, Scalar(255, 0, 0));
	//return make_pair(Point(cx, cy), R);

	//normalize(dist, dst_palm, 0, 1, CV_MINMAX);
}

// 处理一个轮廓
void HandDetector::handle_contour(Mat& dst, const vector<Point> &contour)
{
	vector<Point> hull;
	vector<int> hullsI;
	vector<Vec4i> defects;
	vector<Point> approxCurve;

	Scalar color = Scalar(255, 255, 255);

	// 道格拉斯-普克算法
	approxPolyDP(Mat(contour), approxCurve, 15, true);

	// 计算闭包和凸缺陷
	convexHull(Mat(approxCurve), hull, false);
	convexHull(Mat(approxCurve), hullsI, false);

	// 画出轮廓和凸包
	//draw_contour(dst, contour, color, 2);
	draw_contour(dst, approxCurve, color, 1);
	draw_contour(dst, hull, color, 1);

	// 纠正方向
	ori_correct(dst, contour);

	// 画出凸包点
	//for (int j = 0; j < hullsI.size(); j++)
	//{
	//	int k = hullsI[j];
	//	circle(dst, contour[k], 3, Scalar(0, 255, 0), 2);
	//}

	// 计算凸缺陷
	convexityDefects(Mat(approxCurve), hullsI, defects);
	vector<Vec4i>::iterator d = defects.begin();
	while (d != defects.end()) {
		Vec4i& v = (*d);
		int startidx = v[0];
		Point ptStart(approxCurve[startidx]); // point of the contour where the defect begins
		int endidx = v[1];
		Point ptEnd(approxCurve[endidx]); // point of the contour where the defect ends
		int faridx = v[2];
		Point ptFar(approxCurve[faridx]);// the farthest from the convex hull point within the defect
		int depth = v[3];					// distance between the farthest point and the convex hull

		//if (depth > 20 && depth < 150)
		{
			//line(dst, ptStart, ptFar, CV_RGB(0, 255, 0), 2);
			//line(dst, ptEnd, ptFar, CV_RGB(0, 255, 0), 2);
			circle(dst, ptStart, 4, Scalar(255, 0, 100), 2);
			circle(dst, ptEnd, 4, Scalar(255, 0, 100), 2);
			circle(dst, ptFar, 4, Scalar(100, 0, 255), 2);
		}
		d++;
	}
}

// 检测手部轮廓
void HandDetector::hand_contours(const Mat& src, Mat& dst)
{
	vector<vector<Point> > contours;
	findContours(src, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	if (contours.size() == 0)
		return;

	// only max-area contour valid
	int idx = biggest_contour(contours);
	handle_contour(dst, contours[idx]);

	// 检测手掌
	//Mat palm;
	//detect_palm(src, contours[idx], palm);
	//dst = palm;
	
	//int va_contours = 0;
	//for (int i = 0; i < contours.size(); i++) {
	//	double area = contourArea(contours[i]);
	//	double length = arcLength(contours[i], false);

	//	if (area > THRESHOLD_AREA)
	//	{
	//		va_contours++;
	//		handle_contour(dst, contours[i]);
	//	}
	//}

	//// no hand detect
	//if (va_contours > MAX_CONTOURS_SIZE)
	//	dst = Mat::zeros(src.size(), CV_8UC1);
}