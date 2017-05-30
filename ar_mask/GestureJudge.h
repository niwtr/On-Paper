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

	struct Gesture {
		GestureType type = NONE;
		vector<Point> fingers;
	};

	class GestureJudge {
	public:
		GestureJudge() : _hd(TAR_WIDTH) {

		};

		struct Gesture get_gesture(const Mat& src);

		Mat mask;		// for DEBUG

	private:
		HandDetector _hd;
	};
}