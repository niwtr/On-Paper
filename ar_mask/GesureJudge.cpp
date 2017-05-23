
#include "GesureJudge.h"

on_paper::GestureType on_paper::GestureJudge::get_gesture(const cv::Mat& src)
{
	_hd.process(src, mask);

	vector<Point> fingers = _hd.get_fingers();
	if (fingers.size() > 1)
		return MOVE;
	else if(fingers.size() == 1)
		return PRESS;

	return NONE;
}

cv::Point on_paper::GestureJudge::key_point()
{
	return _hd.get_fingertip();
}