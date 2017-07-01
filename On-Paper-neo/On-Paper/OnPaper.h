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



#ifdef _WIN32

#define and &&
#define or ||
#define not !
#define ROOT "C:/Users/L.Laddie/Documents/On-Paper/ar_mask/"
#else
#define ROOT "../"
#endif // _WIN32

#define elif else if


namespace on_paper {
    class OnPaper {

    private:
        VideoCapture TheVideoCapturer;
        CameraParameters TheCameraParameters;
        Mat TheInputImage;
        Mat TheProcessedImage;

        ARCapturer ac;
        GestureJudge gj;
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
        }

        void camera_start(void);
        void main_loop(void);
        Mat& process_one_frame(void);


private:
        bool allow_write ;
        bool allow_enlarge ;
        bool allow_trigger ;
        int wait_time=1;
        long last_gesture_time;
        GestureType last_gesture;
        int current_page; //TODO move this "page" to static member of Paperfun?

    };



}


#endif //ARTEST_ONPAPER_H
