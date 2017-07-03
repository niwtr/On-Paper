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

	string root = ".";

	HandDetector hd(tar_width);

	if (train)
	{
		cout << "Training Model...\n";

		ifstream fs(root + "\\img_name.txt");
		if (!fs.is_open())
		{
			cerr << "Error: cannot open file\n";
			return -1;
		}

		Mat img, mask;
		while (!fs.eof())
		{
			string name;
			fs >> name;

			img = imread(root + "\\img\\" + name);
			if (!img.data)
				break;

			mask = imread(root + "\\mask\\" + name);
			if (!img.data)
				break;

			//hd.train(img, mask);

			cout << name << endl;

			hd.process(img, mask);
			imshow("mask", mask);
			waitKey(0);
		}
	}
	else
	{
		cout << "Testing Model...\n";
		string video_name = root + "\\test1.mp4";

		VideoCapture cap;
		cap.open(0);
		if (!cap.isOpened()) {
			cerr << "Error: cannot open camera\n";
			return -1;
		}

		Mat img, hand;
		int cnt = 0;
		time_t start, stop;
		start = time(NULL);

		int first = 1;
		namedWindow("Finger_Tip");
		setMouseCallback("Finger_Tip", HandDetector::init_threshold, &hd);

		while (1)
		{
			cap >> img;
            cap >> img;
			if (!img.data)
				break;

			hd.process(img, hand);
			imshow("mask", hand);

			Point finger_tip = hd.get_fingertip();
			circle(img, finger_tip, 4, Scalar(0, 0, 255), 2);
			imshow("Finger_Tip", img);

            if(first)
            {
                waitKey(0);
                setMouseCallback("Finger_Tip", NULL);
                first = 0;
                continue;
            }

			waitKey(1);

			// calculate and display fps every 2s
			cnt++;
			stop = time(NULL);
			if (stop - start > 2)
			{
				cout << "fps: " << cnt / (stop - start) << endl;
				start = time(NULL);
				cnt = 0;
			}
		}
	}
}