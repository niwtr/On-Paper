
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

on_paper::GestureType on_paper::GestureManager::on_gesture_unchanged()
{
    GestureType judgedGT = last_gesture; // return val.
    // accumuate time.
    long curtime = utils::curtime_msec();
    accumulated_time += curtime - last_action_time;
    last_action_time = curtime;

    //status machine.
    switch(state){
    case NOHAND:
        switch(last_gesture){
        /*****************************************************/
        //nohand and last gesture is PRESS? that is impossible!
        case ENLARGE:
        case PRESS:
            goto exit_abnormal;
        /*******************************************************/
        case NONE:
            goto exit; //unchanged, for everything.
        case MOVE:

            //migrate to RECOG?
            if(accumulated_time > 1000){ //one sec
               //ja!
                state=RECOGREADY;
                accumulated_time = 0; //clear accumulated time.
            }
            goto exit;
        }

     case RECOGREADY:
        switch(last_gesture){
        case ENLARGE:
        case PRESS:
        case MOVE:
            if(accumulated_time > 1000){ //one sec.
                state=INACTION;
                accumulated_time = 0;
                stored_gesture = last_gesture;
            }
            goto exit;

        case NONE:
            goto exit_abnormal;
            /*********************************/
            // might be removed.
            // this is not possibly happen.
            if (accumulated_time > 300){ //
                state = NOHAND;
                accumulated_time = 0;
            }
            goto exit;
            /***********************************/

        }
     case INACTION:
        switch(last_gesture){
        case PRESS:
        case ENLARGE:
        case MOVE:
            //the gesture has already changed.
            this->action_gesture_changed = false;
            if(last_gesture != stored_gesture){
                //this time we should consider the acc time.
                //ALERT: not agree with the plan!!!
                if(accumulated_time > 400){ //400 msec.
                    // state is unchaged.
                    stored_gesture = last_gesture; //change gesture!
                    accumulated_time = 0;
                    this->action_gesture_changed = true;
                }
            }

            //exit now!
            judgedGT = stored_gesture;
            goto  exit;
        case NONE:

            /*****************************/
            // this is not possibly happen.
            if(accumulated_time > 300)
            {
                state=NOHAND;
                accumulated_time=0;

            }
            goto exit;
            /******************************/
        }

    }

    exit:
    return judgedGT;

    exit_abnormal:
    exit(999);

}

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
    }

exit:
    return judgedGT;

}
