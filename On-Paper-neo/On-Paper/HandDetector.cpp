#include "HandDetector.h"
#include "cvutils.h"
on_paper::HandDetector::HandDetector(int tar_width) :
	_tar_width(tar_width)
{
}

on_paper::HandDetector::HandDetector(int tar_width, Vec3b min_ycrcb, Vec3b max_ycrcb):
    _tar_width(tar_width), _min_color(min_ycrcb), _max_color(max_ycrcb)
{
}

// 初始化新图
void on_paper::HandDetector::init(int src_width)
{
	_prop = _tar_width * 1.0 / src_width;
	_ctl_point = Point(0, 0);
	_defects.clear();
	_center = Point(0, 0);
	_r = 0;
}

// used otsu + YCrCb filter
void on_paper::HandDetector::process(const cv::Mat& src, cv::Mat& dst)
{
	init(src.size().width);

	Mat src_std;
	resize(src, src_std, Size(_tar_width, _prop * src.size().height));

	dst = Mat::zeros(src_std.size(), CV_8UC3);

	Mat mask;

    if(HAND_MODEL)
        hand_mask2(src_std, mask);
    else
        hand_mask(src_std, mask);
	medianBlur(mask, mask, BLUR_KSIZE);

    Mat element = getStructuringElement(MORPH_RECT, Size(DILATE_SIZE, DILATE_SIZE));
    dilate(mask, mask, element);
	
	hand_contours(mask, src_std);
    dst = mask;
}

// 按比例还原点
cv::Point on_paper::HandDetector::cvt_prop(cv::Point p)
{
	Point rtn;
	rtn.x = p.x / _prop;
	rtn.y = p.y / _prop;

	return rtn;
}

cv::Point on_paper::HandDetector::get_fingertip()
{
	return cvt_prop(_ctl_point);
}

std::vector<on_paper::Vec3p> on_paper::HandDetector::get_defects()
{
	vector<Vec3p> dfts;
	for (Vec3p df : _defects)
		dfts.push_back(Vec3p(cvt_prop(df[0]), cvt_prop(df[1]), cvt_prop(df[2])));

	return dfts;
}

// detect hand mask: method 1
void on_paper::HandDetector::hand_mask(const cv::Mat& src_std, cv::Mat& dst)
{
	cvtColor(src_std, src_std, CV_BGR2YCrCb);

	vector<Mat> src_colors;
	split(src_std, src_colors);

	Mat thd;
	threshold(src_colors[1], thd, 0, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);

	Mat tmp;
	threashold_Y(src_std, tmp);

	bitwise_and(tmp, thd, dst);
}

// detect hand mask: method 2
void on_paper::HandDetector::hand_mask2(const cv::Mat& src_std, cv::Mat& dst)
{
    cvtColor(src_std, src_std, COLOR_TO_TYPE);

    threashold_color(src_std, dst);
}

// color filter with different kind of color space type
void on_paper::HandDetector::threashold_color(const cv::Mat& src, cv::Mat& dst)
{
	dst = Mat::zeros(src.size(), CV_8UC1);

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			Vec3b ele = src.at<Vec3b>(i, j);
            if(ele[0] > _min_color[0] && ele[0] < _max_color[0] && ele[1] > _min_color[1] &&
                    ele[1] < _max_color[1] && ele[2] > _min_color[2] && ele[2] < _max_color[2])
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
void on_paper::HandDetector::threashold_Y(const cv::Mat& src, cv::Mat& dst)
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
int on_paper::HandDetector::biggest_contour(std::vector<std::vector<cv::Point> > &contours)
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
void on_paper::HandDetector::draw_contour(cv::Mat& dst, const std::vector<cv::Point> &contour, const cv::Scalar& color, int thickness)
{
	vector<vector<Point> > tmp_points;
	tmp_points.push_back(contour);
	drawContours(dst, tmp_points, 0, color, 1);
}



// 矫正手部方向
void on_paper::HandDetector::ori_correct(cv::Mat& dst, const std::vector<cv::Point> &contour)
{
	Vec4f li;
	fitLine(contour, li, CV_DIST_HUBER, 0, 0.01, 0.01);
	double cos_theta = li[0];
	double sin_theta = li[1];
	double x0 = li[2], y0 = li[3];

	double phi = atan2(sin_theta, cos_theta) + utils::PI / 2.0;
	double rho = y0 * cos_theta - x0 * sin_theta;

    utils::drawLine(dst, phi, rho, Scalar(0));
}

// 检测手掌
void on_paper::HandDetector::detect_palm(const cv::Mat& src_hand, const std::vector<cv::Point>& contour)
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

	_center = Point(cx, cy);
	_r = R;

	//normalize(dist, dst_palm, 0, 1, CV_MINMAX);
}


// 处理一个轮廓
void on_paper::HandDetector::handle_contour(cv::Mat& dst, const std::vector<cv::Point> &contour)
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

    //int y_high = dst.rows;
    //int y_low = 0, y_mean;
    //for(const auto& I : hullsI){
    //    y_high = approxCurve[I].y<y_high?approxCurve[I].y:y_high;
    //    y_low = approxCurve[I].y>y_low?approxCurve[I].y:y_low;
    //} y_mean = (y_high+y_low)/2;

	for (int j = 0; j < hullsI.size(); j++)
	{
		int k = hullsI[j];
   //     if(approxCurve[k].y > y_mean)
			//continue;

		// 掌心以下的点不作为指尖
		if (approxCurve[k].y > _center.y)
			continue;

		// 寻找最远指尖坐标
		tmp = utils::distance_P2P(_center, approxCurve[k]);
		if (tmp_max < tmp)
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

		if (utils::get_angle(pt_s, pt_f, pt_e) < THRESHOLD_ANGLE)
		{
			circle(dst, pt_s, 4, Scalar(255, 0, 100), 2);
			circle(dst, pt_e, 4, Scalar(255, 0, 100), 2);
			circle(dst, pt_f, 4, Scalar(100, 0, 255), 2);

			_defects.push_back(Vec3p(pt_s, pt_f, pt_e));
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
void on_paper::HandDetector::hand_contours(const cv::Mat& src, cv::Mat& dst)
{
	vector<vector<Point> > contours;
	findContours(src, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	// 未检测到手
	if (contours.size() == 0 || contours.size() > MAX_CONTOURS_SIZE)
		return;

	// 检测最大轮廓
	int idx = biggest_contour(contours);

	// 检测手掌
	detect_palm(src, contours[idx]);
	circle(dst, _center, _r, Scalar(255, 0, 0));

	// 处理轮廓	
	handle_contour(dst, contours[idx]);

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
