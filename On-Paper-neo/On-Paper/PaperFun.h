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
#include <nlohmann/json.hpp>
#include "cvutils.h"
#include "Painter.h"
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <functional>
#include <unordered_map>
#include "defs.h"
#include "archiver.h"

namespace on_paper
{
    using namespace cv;
    using namespace std;
    using namespace nlohmann;

    class Info{
    public:
        Point tl,br;
        string function;
        string data;
        Point finger ; //FIXME 这只是一个ad-hoc的解决方案。
        // 良好的设计应该是把附加的参数（比如当前所指的地点finger）与json的这个结构体分开。
    };



    class PaperFun {

    public:
        //Mat appended_fun_layer;
        Mat transmatrix;
    private:
        Painter * pa_ptr;
        int paper_width = 2480;
        int paper_height = 3508;
        map<int,vector<Info>> json_parse;
        Mat picture;
        json j;
        using functor = function<void(Info)>;
        unordered_map<string, functor> _fnmap;
    public:
        //at the very start...
        void initialize(void){
            pic_name = "";
            register_callbacks();
        }
        //at each time the page changes.
        void load_archiv_conf(archiv_conf acnf); // init the whole book.
        inline void set_page_size(int w, int h){this->paper_width=w;this->paper_height=h;}
        inline void capture_Painter(Painter * pa){this->pa_ptr = pa;}
        void register_callbacks(void);
        void fire_event(vector<Point> figPs, Point &, int page);
        void call_paper_fun(string function_name, Info arg);
        Point pixtransform(Point & p);

    private:
        void parseJson();
        void transform_point(Point& p);
        bool judgeIn(Point p,Point tl,Point br);

    private://func storage.
        string pic_name; // the picture path.
        archiv_conf aconf; //configure information of archiv.


    };

}
#endif //AR_MASK_APPENDEDFUN_H
