// hand mask detector

#include <iostream>
#include <cstdio>
#include <direct.h>
#include <opencv2/opencv.hpp>

#include "HandDetector.h"

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	int train = 0;				// train or test
	int tar_width = 500;		// resizing the input image

	char buffer[_MAX_PATH];
	getcwd(buffer, _MAX_PATH);
	string root = buffer;

	if (train)
	{
		cout << "Training Model...\n";
	}
	else
	{
		cout << "Testing Model...\n";
		string video_name = root + "\\test.mp4";

		VideoCapture cap;
		cap.open(video_name);
		if (!cap.isOpened()) {
			cerr << "Error: cannot open camera\n";
			return -1;
		}

		HandDetector hd(tar_width);
		Mat img, mask;
		while (1)
		{
			cap >> img;
			if (!img.data)
				break;

			hd.process(img, mask);
			imshow("mask", mask);
			waitKey(1);
		}
	}
}