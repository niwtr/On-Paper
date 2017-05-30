
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