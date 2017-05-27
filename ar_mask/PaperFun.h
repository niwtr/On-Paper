//
// Created by xuan on 17-5-25.
//

#ifndef AR_MASK_APPENDEDFUN_H
#define AR_MASK_APPENDEDFUN_H
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "aruco/aruco.h"
#include "json.hpp"
#include "cvutils.h"
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define CONFIGPATH "/home/xuan/下载/poem/config/"
#define ZERONUM 4

namespace on_paper
{
    using namespace cv;
    using namespace std;
    using namespace nlohmann;
    class Info{
    public:
        Point tl,br;
        string pic_path;
    };
    class PaperFun {
    public:
        Mat appended_fun_layer;
        Mat picture;
        json j;
        Mat transmatrix;
    private:
        vector<Info> json_parse;
    public:
        void init(int page);
        void showPic(vector<Point> figPs);
        Mat get_pic_layer(){return this->appended_fun_layer;};
    private:
        void parseJson();
        void transform_point(Point& p);
    };
}



#endif //AR_MASK_APPENDEDFUN_H
