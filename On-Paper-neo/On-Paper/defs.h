#ifndef DEFS_H
#define DEFS_H

#define ROOT "../"
#define HAND_MODEL 0

//#define MAIN_WIDTH 700
//#define MAIN_HEIGHT 550
#define MAIN_WIDTH 1024
#define MAIN_HEIGHT 768
// ARCapturer.h
#define IMAGEPATH "/home/heranort/Dev/example/images/"
#define MARKERNUM 4
#define ZERONUM 1


// beforestart.h
#define THRSD_UPDOWN 10


// HandDetector.h
#define MIN_Cr 113
#define MAX_Cr 173
#define MIN_Cb 77
#define MAX_Cb 127
#define MIN_Y 30

#define THRESHOLD_AREA 20
#define MAX_CONTOURS_SIZE 8
#define THRESHOLD_ANGLE 90
#define BLUR_KSIZE 5
#define DILATE_SIZE 4

// used for hand model 2
// include type: CV_BGR2RGB/CV_BGR2YCrCb/CV_BGR2YUV/CV_BGR2HSV_FULL/CV_BGR2Lab
#define COLOR_TO_TYPE CV_BGR2YCrCb


// GestureJudge.h
#define TAR_WIDTH 320


// PaperFun.h
#define CONFIGPATH "/home/heranort/Dev/example/config/"
//#define CONFIGPATH "/home/heranort/Dev/prince/config/"
#define PICTUREPATH "/home/heranort/Dev/example/picture/"
//#define PICTUREPATH ""
#define ZERONUM 1

#endif // DEFS_H
