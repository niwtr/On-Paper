#include "HandDetector.h"

HandDetector::HandDetector(int tar_width) :
	_tar_width(tar_width)
{
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
	_ctl_point = Point(0, 0);	// 清零
	_prop = _tar_width * 1.0 / src.size().width;

	Mat src_std;
	resize(src, src_std, Size(_tar_width, _prop * src.size().height));

	dst = Mat::zeros(src_std.size(), CV_8UC3);

	Mat mask;
	hand_mask(src_std, mask);
	medianBlur(mask, mask, BLUR_KSIZE);
	
	hand_contours(mask, src_std);
	dst = src_std;
}

Point HandDetector::get_fingertip()
{
	Point rtn;
	rtn.x =  _ctl_point.x / _prop;
	rtn.y = _ctl_point.y / _prop;
	return rtn;
}

// detect hand mask: method 1
void HandDetector::hand_mask(const Mat& src_std, Mat& dst)
{
	cvtColor(src_std, src_std, CV_BGR2YCrCb);

	vector<Mat> src_colors;
	split(src_std, src_colors);

	Mat thd;
	threshold(src_colors[1], thd, 0, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);

	Mat tmp;
	threashold_Y(src_std, tmp);

	Mat img_and;
	bitwise_and(tmp, thd, dst);
}

// detect hand mask: method 2
void HandDetector::hand_mask2(const Mat& src_std, Mat& dst)
{
	cvtColor(src_std, src_std, CV_BGR2YCrCb);

	threashold_YCrCb(src_std, dst);
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
		Point pt1(rho / cos(theta), 0);
		Point pt2((rho - image.rows * sin(theta)) / cos(theta), image.rows);
		line(image, pt1, pt2, Scalar(255), 1);
	}
	else
	{
		Point pt1(0, rho / sin(theta));
		Point pt2(image.cols, (rho - image.cols * cos(theta)) / sin(theta));
		line(image, pt1, pt2, color, 1);
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

	drawLine(dst, phi, rho, Scalar(0));
}

// 检测手掌
void HandDetector::detect_palm(const Mat& src_hand, const vector<Point>& contour, Point& center, int& r)
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

	center = Point(cx, cy);
	r = R;

	//normalize(dist, dst_palm, 0, 1, CV_MINMAX);
}

// 两点距离
float HandDetector::distance_P2P(Point a, Point b) {
	float d = sqrt(fabs(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)));
	return d;
}

// 获得三点角度(弧度制)
float HandDetector::get_angle(Point s, Point f, Point e) {
	float l1 = distance_P2P(f, s);
	float l2 = distance_P2P(f, e);
	float dot = (s.x - f.x)*(e.x - f.x) + (s.y - f.y)*(e.y - f.y);
	float angle = acos(dot / (l1*l2));
	angle = angle * 180 / PI;
	return angle;
}

// 处理一个轮廓
void HandDetector::handle_contour(Mat& dst, const vector<Point> &contour, const Point center, const int r)
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

	// 未检测到手指（手掌闭合状态）
	//if (matchShapes(approxCurve, hull, CV_CONTOURS_MATCH_I1, 0) == 0)
//		return;

	// 纠正方向
	//ori_correct(dst, contour);

	// 找距离掌心最远凸包点
	float tmp_max = 0, tmp = 0;
	Point far_max(0, 0);
	for (int j = 0; j < hullsI.size(); j++)
	{
		int k = hullsI[j];

		tmp = distance_P2P(center, approxCurve[k]);
		if (tmp_max < tmp && approxCurve[k].x != dst.cols-1 && approxCurve[k].y != dst.rows-1) // 指尖不会出现在边缘
		{
			tmp_max = tmp;
			far_max = approxCurve[k];
		}
	}
	
	// 计算凸缺陷
	convexityDefects(Mat(approxCurve), hullsI, defects);
	vector<Vec4i>::iterator d = defects.begin();
	while (d != defects.end()) {
		Vec4i& v = (*d);
		Point pt_s(approxCurve[v[0]]);		// point of the contour where the defect begins
		Point pt_e(approxCurve[v[1]]);		// point of the contour where the defect ends
		Point pt_f(approxCurve[v[2]]);		// the farthest from the convex hull point within the defect
		int depth = v[3];					// distance between the farthest point and the convex hull

		if (get_angle(pt_s, pt_f, pt_e) < THRESHOLD_ANGLE)
		{
			circle(dst, pt_s, 4, Scalar(255, 0, 100), 2);
			circle(dst, pt_e, 4, Scalar(255, 0, 100), 2);
			circle(dst, pt_f, 4, Scalar(100, 0, 255), 2);
		}
		d++;
	}

	if (far_max != Point(0, 0))
	{
		_ctl_point = far_max;
		circle(dst, far_max, 4, Scalar(0, 255, 0), 2);
	}
}

// 检测手部轮廓
void HandDetector::hand_contours(const Mat& src, Mat& dst)
{
	vector<vector<Point> > contours;
	findContours(src, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	// 未检测到手
	if (contours.size() == 0 || contours.size() > MAX_CONTOURS_SIZE)
		return;

	// 检测最大轮廓
	int idx = biggest_contour(contours);

	// 检测手掌
	Point center;
	int r;
	detect_palm(src, contours[idx], center, r);
	circle(dst, center, r, Scalar(255, 0, 0));

	// 处理轮廓	
	handle_contour(dst, contours[idx], center, r);

	// 画出所有轮廓
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