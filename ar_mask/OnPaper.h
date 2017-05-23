//
// Created by 牛天睿 on 17/5/22.
//

#ifndef ARTEST_ONPAPER_H
#define ARTEST_ONPAPER_H

#include "ARCapturer.h"
#include "HandDetector.h"
#include "Painter.h"

namespace on_paper {
    class OnPaper {
    public:
        int hd_width;
    private:
        VideoCapture TheVideoCapturer;
        CameraParameters TheCameraParameters;
        Mat TheInputImage;
        Mat TheProcessedImage;
        ARCapturer ac;
        //TODO 把HandDetector加入OnPaper命名空间。
        HandDetector hd;
        Painter pa;

    public:
        //TODO 为构造函数添加合适的参数
        OnPaper(int hdwidth = 200):hd_width(hdwidth),hd(hd_width){}
        void init(void){
            TheCameraParameters.readFromXMLFile("../camera.yml");
            ac.init(TheCameraParameters);
            const Mat& img = ac.get_image();
            pa.init(img.rows, img.cols);

        }
        void main_loop(void);


    };
}


#endif //ARTEST_ONPAPER_H
