#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

#include "HandDetector.h"
#include "defs.h"

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

        }

        // only used for hand mask model 2
        void set_hand_thrsd(Vec3b min_thrsd, Vec3b max_thrsd){
            _hd.set_thrsd(min_thrsd, max_thrsd);
        }

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
        ACTIONPAUSE
    };

 class GestureManager {
    private:
        GestureJudge * judge;
        GMState state;
        GestureType last_gesture;
        GestureType stored_gesture;
        long last_action_time; // last time for action
        long accumulated_time;
        int percentage;
        string msg;



    public: // public methods
        explicit GestureManager(GestureJudge* gj);

        /* status machine. */
        GestureType get_uber_gesture(GestureType gt); // revised gesture.
        GMState get_state(){ return this->state; }
        bool action_gesture_changed;
        int get_progress_percentage(){return this->percentage;}
        string get_progress_message(){return this->msg;}

    private: //private methods
        GestureType on_gesture_unchanged();
        GestureType on_gesture_changed(GestureType changedTo);

 private:

    };





















}
