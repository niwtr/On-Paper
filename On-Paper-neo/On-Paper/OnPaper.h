//
// Created by 牛天睿 on 17/5/22.
//

#ifndef ARTEST_ONPAPER_H
#define ARTEST_ONPAPER_H

#include "ARCapturer.h"
#include "HandDetector.h"
#include "Painter.h"
#include "GestureJudge.h"
#include "LayerManager.h"
//#include "ToolBox.h"
#include "PaperFun.h"
#include <QDir>

#include "glcanvas.h"
#include "beforestart.h"
#include "defs.h"
#include <QZXing.h>
#include <string>
#include "archiver.h"
using BarCodeDecoder=QZXing;
using std::string;
#define elif else if


namespace on_paper {
enum op_status {
    op_normal,
    op_barcode,
};
class OnPaper {

private:
    VideoCapture TheVideoCapturer;
    CameraParameters TheCameraParameters;
    Mat TheInputImage;
    Mat TheProcessedImage;
    BeforeStart bs;
    string last_good_barcode;

public:
    ARCapturer ac;
    GestureJudge gj;
    GestureManager * gm;
    LayerManager lm;
    Painter pa;
    //ToolBox tb;
    PaperFun af;
    BarCodeDecoder bcd;
    Archiver axv;
public:

    OnPaper(){}
    void init(void);
    void train_hand_thrsd();
    void camera_start(void);
    Mat& process_one_frame(void);

private:
    int current_page; //TODO move this "page" to static member of Paperfun?
    Mat& _process_normal(void);
    Mat& _process_barcode(void);
public:

    int wait_time=1;
    long last_gesture_time;
    GestureType last_gesture;


    bool allow_write ;
    bool allow_enlarge ;
    bool allow_triggers ;



    op_status status;
};



}


#endif //ARTEST_ONPAPER_H
