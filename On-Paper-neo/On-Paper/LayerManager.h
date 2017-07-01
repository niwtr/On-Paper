//
// Created by 牛天睿 on 17/5/23.
//

#ifndef ARTEST_LAYERMANAGER_H
#define ARTEST_LAYERMANAGER_H

#include "ARCapturer.h"
#include "cvutils.h"
#include <queue>
//TODO 把命名空间声明放到一个单独的文件里？
namespace on_paper {

    class LayerManager {
    public:
        LayerManager(){}
        void capture(const Mat& m){ layers.push(m); }
        void overlay(){
            int i=0;
            while(not layers.empty()){
                auto& m = layers.front();
                if(_layer.empty()) {
                    _layer = m;
                    goto next;
                }
                else if(m.size()!= _layer.size()){
                    goto next;
                }
                else {

                    if(m.channels() == 4)//BGRA
                    {
                        on_paper::utils::overlay_BGRA(_layer, m, _layer, Point(0, 0));
                    }
                    else if(m.channels() == 3) //BGR
                    {
                        if(_layer.channels() != 3)
                            cv::cvtColor(_layer, _layer, CV_BGRA2BGR);
                        on_paper::utils::overlay_BGR(_layer, m, _layer);
                    }
                    goto next;
                }
                next:
                layers.pop();
            }
        }
        //release the matrix and empty it.
        void output(Mat& m){ _layer.copyTo(m); _layer=Mat();}

    private:
        std::queue<Mat> layers;
        Mat _layer;



    };
}


#endif //ARTEST_LAYERMANAGER_H
