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

#define elif else if


namespace on_paper {
class OnPaper {

private:
    VideoCapture TheVideoCapturer;
    CameraParameters TheCameraParameters;
    Mat TheInputImage;
    Mat TheProcessedImage;
    BeforeStart bs;

public:
    ARCapturer ac;
    GestureJudge gj;
    GestureManager * gm;
    LayerManager lm;
    Painter pa;
    //ToolBox tb;
    PaperFun af;
public:
    OnPaper(){}
    void init(void){
        //TheCameraParameters.readFromXMLFile(string(ROOT) + "camera.yml"); //outdated.
        TheCameraParameters.readFromXMLFile("./camera.yml");
        ac.init(TheCameraParameters);
        const Mat& img = ac.get_image();
        pa.init(img.rows, img.cols);
        af.initialize();
        ac.capture_Painter(&pa);
        af.capture_Painter(&pa);
        gm = new GestureManager(&gj);
    }

    void train_hand_thrsd();
    void camera_start(void);
    Mat& process_one_frame(void);

private:
    int current_page; //TODO move this "page" to static member of Paperfun?

public:

    int wait_time=1;
    long last_gesture_time;
    GestureType last_gesture;


    bool allow_write ;
    bool allow_enlarge ;
    bool allow_triggers ;
};



}


#endif //ARTEST_ONPAPER_H
