#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

#include "HandDetector.h"

#define TAR_WIDTH 320

namespace on_paper {
	using namespace cv;
	using namespace std;

	enum GestureType {
        PRESS,//one
        MOVE,//>2
        ENLARGE,//2
        NONE//nil
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

    // higher-level manager for sequentially managing gestures.


    // gesture manager state
    enum GMState{
        NOHAND,
        RECOGREADY,
        INACTION,
    };

 class GestureManager {
    private:
        GestureJudge * judge;
        GMState state;
        GestureType last_gesture;
        GestureType stored_gesture;
        long last_action_time; // last time for action
        long accumulated_time;


    public: // public methods
        explicit GestureManager(GestureJudge* gj);

        /* status machine. */
        GestureType get_uber_gesture(GestureType gt); // revised gesture.

    private: //private methods
        GestureType on_gesture_unchanged();
        GestureType on_gesture_changed(GestureType changedTo);

 private:

    };





















}
