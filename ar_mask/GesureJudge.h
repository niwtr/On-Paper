#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

#include "HandDetector.h"

#define TAR_WIDTH 320

namespace on_paper {
	using namespace cv;
	using namespace std;

	enum GestureType {
		PRESS,
		MOVE,
		ENLARGE,
		NONE
	};

	class GestureJudge {
	public:
		GestureJudge() : _hd(TAR_WIDTH) {

		};

		GestureType get_gesture(const Mat& src);
		Point key_point();

		Mat mask;		// for DEBUG

	private:
		HandDetector _hd;
	};
}