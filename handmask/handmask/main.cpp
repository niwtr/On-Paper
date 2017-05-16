// hand mask detector

#include <iostream>
#include <cstdio>
#include <opencv2/opencv.hpp>

#include "HandDetector.h"

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
	int train = 0;				// train or test
	int tar_width = 320;		// resizing the input image


	if (train)
	{
		cout << "Training Model...\n";
	}
	else
	{
		cout << "Testing Model...\n";
		string video_name = "C:/Users/WORK/Desktop/test.avi";

		VideoCapture cap;
		cap.open(video_name);
		if (!cap.isOpened()) {
			cerr << "Error: cannot open camera\n";
			return -1;
		}

		HandDetector hd;
		Mat img, mask;
		while (1)
		{
			cap >> img;

			hd.process(img, mask);
			imshow("mask", img);
			waitKey(1);
		}
	}
}