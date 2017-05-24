
#include "GesureJudge.h"
#include "cvutils.h"
on_paper::GestureType on_paper::GestureJudge::get_gesture(const cv::Mat& src)
{
	_hd.process(src, mask);

	vector<Vec3p> defects = _hd.get_defects();
	if (defects.size() > 1)
		return MOVE;
	else if (defects.size() == 0 && _hd.get_fingertip() != Point(0, 0))
		return PRESS;
	else if (defects.size() == 1) {
		Vec3p dft = defects[0];
		float f1 = utils::distance_P2P(dft[0], dft[1]);
		float f2 = utils::distance_P2P(dft[2], dft[1]);

		if (f1 / f2 > 1.5 || f2 / f1 > 1.5) {
			return ENLARGE;
		}
		else
			return MOVE;
	}

	return NONE;
}

cv::Point on_paper::GestureJudge::key_point()
{
	return _hd.get_fingertip();
}