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
#include "ToolBox.h"
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
        ToolBox tb;

    public:
        OnPaper(){}
        void init(void){
            TheCameraParameters.readFromXMLFile(string(ROOT) + "camera.yml");
            ac.init(TheCameraParameters);
            const Mat& img = ac.get_image();
            pa.init(img.rows, img.cols);

        }
        void main_loop(void);


    };
}


#endif //ARTEST_ONPAPER_H
