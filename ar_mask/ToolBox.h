//
// Created by heranort on 17-5-25.
//

#ifndef ARTEST_TOOLBOX_H
#define ARTEST_TOOLBOX_H


#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <iostream>
#include <opencv/cv.hpp>
#include <vector>
#include <opencv2/highgui.hpp>
#include "cvutils.h"
#include <map>
#include <string>
#include <functional>
namespace on_paper {
    using namespace cv;
    using std::vector;
    using std::string;
    using callback=std::function<void()>;
    using std::move;
    using std::pair;

    class _callback_cell {
        string _name;
        pair<Point, Point> _area; //lt, rb
        callback _cb;
    public:
        _callback_cell(string name, pair<Point, Point> ar, callback fn)
                :_name(name), _area(ar), _cb(move(fn)){}
        bool within(Point p){
            return p.x>_area.first.x
                   and p.x<_area.second.x
                   and p.y>_area.first.y
                   and p.y<_area.second.y;
        }
        void operator()(void){ _cb(); }
    };


    class ToolBox {


    private:
        int swidth;
        int sheight;

        Mat screen;
        vector<_callback_cell> callbacks;

    public:
        void init(int screen_height, int screen_width) {
            this->swidth = screen_width;
            this->sheight = screen_height;
            this->screen = Mat::zeros(screen_height, screen_width, CV_8UC3);
        }

        void register_callback(string name, Point lt, Point rb, callback fn) {
            auto _cbc =
                    _callback_cell(name,
                                   std::make_pair<Point,Point>(move(lt), move(rb)),
                                   fn);
            callbacks.emplace_back(move(_cbc));
        }

        void add_toolbox_icon(Point lt, Point rb, Mat original){
            auto rt = Point(rb.x, lt.y);
            auto lb = Point(lt.x, rb.y);
            vector<Point2f> origin_pat = {
                    Point(0,0),
                    Point(0, original.cols),
                    Point(original.rows, original.cols),
                    Point(original.rows, 0)
            }, aim_pat = {
                    lt,rt,rb,lb
            };
            Mat m = getPerspectiveTransform(origin_pat, aim_pat);
            warpPerspective(original, this->screen, m, screen.size(), INTER_CUBIC);
        }


        void example_dilette(){
            vector<Point> corners{
                    Point(0,0),
                    Point(200,0),
                    Point(200,150),
                    Point(0,150)
            }, corners2{
                    Point(200,0),
                    Point(400,0),
                    Point(400,150),
                    Point(200,150)
            };

            vector<vector<Point>> c_corners = {corners},
            c_corners2={corners2};

            fillPoly(screen, c_corners, Scalar(0, 255, 0));
            fillPoly(screen, c_corners2, Scalar(0, 0, 255));
        }

        //return true if an event is performed or else false.
        bool fire_event(const Point& p){
            for(auto & cell : callbacks)
                if(cell.within(p))//within the area
                {
                    cell();
                    return true;
                }
            return false;
        }

        Mat get_toolbox_layer(){ return this->screen; }

    };
}

#endif //ARTEST_TOOLBOX_H
