
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
#include <unordered_map>
using BarCodeDecoder=QZXing;
using std::string;
#define elif else if

struct __w_callback_arg{
    int _int;
    long _long;
    string _string;
};

#include <functional>
using w_callback = std::function<void(struct __w_callback_arg)>;
using w_hash = std::unordered_map<string, w_callback>;

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
    w_hash callbacks;

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
    void register_callback(string cbname, w_callback cb);
    void wcall(string cbname, __w_callback_arg arg);

private:
    int current_page; //TODO move this "page" to static member of Paperfun?

    __w_callback_arg warg;
    Mat& _process_normal(void);
    Mat& _process_barcode(void);

public:

    int wait_time=1;
    long last_gesture_time;
    GestureType last_gesture;
    GMState last_state;

    bool allow_write ;
    bool allow_enlarge ;
    bool allow_triggers ;



    op_status status;
};



}


#endif //ARTEST_ONPAPER_H
