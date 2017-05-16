#include "HandDetector.h"

void HandDetector::process(const Mat& src, Mat& des)
{
	if (!des.data)
		des.create(src.size(), CV_8UC1);
}

template< ColorSpaceType color_type, int win_size>
LcColorComputer<color_type, win_size>::LcColorComputer()
{
	if (win_size == 1) dim = 3;
	else
	{
		dim = win_size*win_size - (win_size - 2)*(win_size - 2);
		dim = dim * 3;
	}
	bound = (win_size - 1) / 2;
}

template< ColorSpaceType color_type, int win_size>
void LcColorComputer<color_type, win_size>::compute(Mat & src, vector<KeyPoint> & keypts, Mat & desc)
{
	double t = double(getTickCount());

	int code;
	if (color_type == LC_YCrCb) code = CV_BGR2YCrCb;
	else if (color_type == LC_HSV) code = CV_BGR2HSV_FULL;
	else if (color_type == LC_LAB) code = CV_BGR2Lab;

	Mat color;
	cvtColor(src, color, code);

	for (int k = 0; k<(int)keypts.size(); k++)
	{
		int r = int(floor(.5 + keypts[k].pt.y) - floor(win_size*0.5));	// upper-left of patch
		int c = int(floor(.5 + keypts[k].pt.x) - floor(win_size*0.5));
		int a = 0;

		for (int i = 0; i<win_size; i++)
		{
			for (int j = 0; j<win_size; j++)
			{
				if (i == 0 || j == 0 || i == win_size - 1 || j == win_size - 1)
				{
					desc.at<float>(k, a + 0) = color.at<Vec3b>(r + i, c + j)(0) / 255.f;
					desc.at<float>(k, a + 1) = color.at<Vec3b>(r + i, c + j)(1) / 255.f;
					desc.at<float>(k, a + 2) = color.at<Vec3b>(r + i, c + j)(2) / 255.f;
					a += 3;
				}
			}
		}
	}

	if (0 && veb) {
		t = (getTickCount() - t) / getTickFrequency();
		if (color_type == LC_HSV)  cout << " copy HSV features:" << t << " secs." << endl;
		else if (color_type == LC_LAB)  cout << " copy LAB features:" << t << " secs." << endl;
	}
}