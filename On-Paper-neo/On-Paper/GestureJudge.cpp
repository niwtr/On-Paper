
#include "GestureJudge.h"
#include "cvutils.h"

struct on_paper::Gesture on_paper::GestureJudge::get_gesture(const cv::Mat& src)
{
	_hd.process(src, mask);
	vector<Vec3p> defects = _hd.get_defects();

	struct Gesture gt;
	auto add_fg = [&gt](Point pt) {
		if (find(gt.fingers.begin(), gt.fingers.end(), pt) == gt.fingers.end())
			gt.fingers.push_back(pt);
	};

    // judge gesture type by the number fingers
	if (defects.size() > 1)
	{
		gt.type = MOVE;
		for (const auto& defect : defects)
		{
			add_fg(defect[0]);
			add_fg(defect[2]);
		}
	}
	else if (defects.size() == 0 && _hd.get_fingertip() != Point(0, 0))
	{
		gt.type = PRESS;
		gt.fingers.push_back(_hd.get_fingertip());
	}
	else if (defects.size() == 1) {
		gt.type = ENLARGE;
		gt.fingers.push_back(defects[0][0]);
		gt.fingers.push_back(defects[0][2]);
	}

    return gt;
}
on_paper::GestureManager::GestureManager(on_paper::GestureJudge *gj)
{

    this->judge=gj;
    this->state=NOHAND;
    this->last_action_time=utils::curtime_msec();
    this->accumulated_time=0;
    this->last_gesture=NONE;
    this->stored_gesture=PRESS;
}

on_paper::GestureType on_paper::GestureManager::get_uber_gesture(on_paper::GestureType gt)
{
    GestureType thisGT = gt;

    if (last_gesture==thisGT)
        return on_gesture_unchanged(); // migration of state must appear on this.
    else
        return on_gesture_changed(thisGT);
}
#define UPDATE_PERCENTAGE(X) percentage=(int)((double)accumulated_time / (double)X * (double) 100)
on_paper::GestureType on_paper::GestureManager::on_gesture_unchanged()
{
    GestureType judgedGT = last_gesture; // return val.

    //accumulate frame.
    accumulated_time ++;


    //status machine.
    switch(state){
    case NOHAND:
        switch(last_gesture){
        //nohand and last gesture is PRESS? that is impossible!
        case ENLARGE:
        case PRESS:
            goto exit_abnormal;

        case NONE:
            msg="Place your hand here.";
            goto exit; //unchanged, for everything.
        case MOVE:

            //migrate to RECOG?

            msg="Recognizing hands ...";
            UPDATE_PERCENTAGE(10);

            if(accumulated_time > 10){ // 10 frames
               //ja!
                state=RECOGREADY;
                accumulated_time = 0; //clear accumulated time.
            }
            goto exit;
        }

     case RECOGREADY:
        msg="Recognizing gesture ...";
        switch(last_gesture){
        case ENLARGE:
        case PRESS:
        case MOVE:

            UPDATE_PERCENTAGE(10);
            if(accumulated_time > 10){ //10 frames
                state=INACTION;
                accumulated_time = 0;
                stored_gesture = last_gesture;
            }
            goto exit;

        case NONE:

            UPDATE_PERCENTAGE(4);
            if (accumulated_time > 4){
                state = NOHAND;
                accumulated_time = 0;
            }
            goto exit;

        }
     case ACTIONPAUSE:
        msg="Action paused.";
        switch(last_gesture){
        case ENLARGE:
            goto exit; //unhandled.
        case PRESS:
            UPDATE_PERCENTAGE(5);
            if(accumulated_time > 5){
                state=INACTION;
                accumulated_time=0;
                stored_gesture = last_gesture;
            }
            goto exit;
        case MOVE:
            UPDATE_PERCENTAGE(5);
            if(accumulated_time > 5){
                state=RECOGREADY;
                accumulated_time = 0;
            }
            goto exit;
        case NONE:
            UPDATE_PERCENTAGE(5);
            if(accumulated_time > 5){
                state=NOHAND;
                accumulated_time=0;
            }
            goto exit;
        }

     case INACTION:
        msg="In action.";
        switch(last_gesture){
        case PRESS:
        case ENLARGE:
        case MOVE:
            //the gesture has already changed.
            //TODO this one(action_gesture_changed) is far from elegant.
            //TODO will be removed from future.
            this->action_gesture_changed = false;

            if(last_gesture != stored_gesture){
                UPDATE_PERCENTAGE(3);
                //this time we should consider the acc time.
                if(accumulated_time > 3){ // 5 frames
                    // state is unchaged, for 5 frames.
                    accumulated_time = 0;
                    if(stored_gesture == PRESS)
                        state=ACTIONPAUSE;
                    else
                        state=RECOGREADY;
                    this->action_gesture_changed = true;
                }
            }

            //exit now!
            judgedGT = stored_gesture;
            goto  exit;
        case NONE:

            UPDATE_PERCENTAGE(4);
            // this is not possibly happen.
            if(accumulated_time > 4) //4 frames
            {
                state=NOHAND;
                accumulated_time=0;
            }
            goto exit;
        }
    }

    exit:
    return judgedGT;

    exit_abnormal:
    exit(999);

}

// the accumulating proc of gm.
on_paper::GestureType on_paper::GestureManager::on_gesture_changed(on_paper::GestureType changedTo)
{

    GestureType judgedGT = changedTo;

    switch(state){
    case GMState::NOHAND:
        switch(changedTo){
        case PRESS:    //ignored
        case ENLARGE:  //ignored
            judgedGT = last_gesture ;//ignore that gesture!
            goto exit;
        case NONE: //lg must be MOVE.
        case MOVE: //lg must be NONE.
            accumulated_time = 0; //clear acc.
            last_gesture = changedTo;
            goto exit; //return changedTo
        }
    case GMState::RECOGREADY:
        switch (changedTo) {
        case PRESS://lg must be MOVE or ENLARGE
        case ENLARGE: //lg must be PRESS or MOVE
        case MOVE: // lg must be PRESS or ENLARGE
            accumulated_time = 0; // clear acc
            last_gesture = changedTo;
            goto exit;
        case NONE: //ignored.
            //discare return val.
            goto exit;
        }
    case GMState::INACTION:
        switch (changedTo) {
        case PRESS://lg: move or enlarge
        case ENLARGE:
        case MOVE:
            accumulated_time = 0;
            last_gesture=changedTo;
            judgedGT = stored_gesture;
            goto exit;
            //possible bug when changeTo=PRESS && last_gesture=ENLARGE or MOVE.
        case NONE:
            // ALERT modify this!!!
            accumulated_time = 0;
            last_gesture = changedTo;
            judgedGT = changedTo;
            goto exit;
        }
    case GMState::ACTIONPAUSE:
        switch(changedTo){
            case MOVE:
            case NONE:
            case PRESS:
            accumulated_time=0;
            last_gesture=changedTo;
            goto exit;
        case ENLARGE:
            goto exit;
        }
    }

exit:
    return judgedGT;

}
