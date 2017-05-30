//
// Created by 牛天睿 on 17/5/22.
//



#include "Painter.h"
using cv::Mat;
using cv::Point;
cv::Mat on_paper::Painter::get_temp_canvas_layer(){
    auto _layer = this->temp_canvas_layer;
    this->temp_canvas = __empty.clone();
    return _layer;
}
void on_paper::Painter::transform_canvas(const Mat& M, Size sz){
    if(M.empty())
        return;
    warpPerspective(canvas, canvas_layer, M, sz, INTER_NEAREST);
    warpPerspective(temp_canvas, temp_canvas_layer, M, sz, INTER_NEAREST);
}

void on_paper::Painter::initKF(){

    const int stat_num = 4;//是什么？
    const int measure_num = 2;//是什么？
    KF.init(stat_num, measure_num, 0);

    KF.transitionMatrix = (Mat_<float>(4, 4) << 1,0,1,0,   0,1,0,1,  0,0,1,0,  0,0,0,1);
    Mat_<float> measurement(2,1); measurement.setTo(Scalar(0));
    measurement_KF=measurement;

    // init...
    KF.statePre.at<float>(0) = 0;
    KF.statePre.at<float>(1) = 0;
    KF.statePre.at<float>(2) = 0;
    KF.statePre.at<float>(3) = 0;
    setIdentity(KF.measurementMatrix);
    setIdentity(KF.processNoiseCov, Scalar::all(1e-4));
    setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));
    setIdentity(KF.errorCovPost, Scalar::all(.1));
}
void on_paper::Painter::init(int rows, int cols) {
    Scalar line_color = Scalar(255, 255, 0);

    this->set_color(line_color);
    this->set_pen_size(5);
    this->canv_height = rows;
    this->canv_width = cols;
    this->canvas = Mat::zeros(rows, cols, CV_8UC3);
    /* init kalman filter */
    this->__empty = Mat::zeros(rows, cols, CV_8UC3);
    this->temp_canvas = __empty.clone();
    initKF();
}
void on_paper::Painter::init_canvas_of_page(int npage){
    auto it = canvas_container.find(npage);
    if(it == canvas_container.end()) {//init new
        Mat newcanvas = Mat::zeros(canv_height, canv_width, CV_8UC3);
        this->canvas = newcanvas;
        this->canvas_container.insert(make_pair<int, Mat>(move(npage), move(newcanvas)));
    } else
        this->canvas = it->second;
}
void on_paper::Painter::draw_line_simple(Point p, Scalar c){
    if(p.x<10 or p.y<10 )
        return;
    if(last_point.x<10 or last_point.y<10){
        last_point = p;
        return;
    }
    line(canvas, last_point, p, c, 2, LINE_AA);
    last_point = p;
}

void on_paper::Painter::transform_point(Point& p){
    vector<Point2f> mreal = {p};
    vector<Point2f> mimage;
    perspectiveTransform(mreal,mimage, this->transmatrix );
    p = mimage[0];
}
cv::Point on_paper::Painter::kalman_smooth(const Point &p){

    //  predict, to update the internal statePre variable
    Mat prediction = KF.predict();
    Point predictPt(prediction.at<int>(0),prediction.at<int>(1));

    //get point and correct
    measurement_KF.at<int>(0) = p.x;
    measurement_KF.at<int>(1) = p.y;

    Point measPt(measurement_KF);
    Mat estimated = KF.correct(measurement_KF);
    Point statePt(estimated.at<int>(0),estimated.at<int>(1));
    return statePt;
}


void on_paper::Painter::kalman_trace(Point p, bool drawp) {
    if(transmatrix.empty())return;
    transform_point(p);

    if(p.x<0 or p.y<0 or p.x > canv_width or p.y > canv_height)
        return;
    if(last_point.x<10 or last_point.y<10
            or
       last_point.x > canv_width or last_point.y > canv_height){
        last_point = p;
        return;
    }
    Point statePt = kalman_smooth(p);
    if(drawp)
        line(canvas, last_point, statePt, _color, _pen_size, LINE_AA);
    last_point =statePt;
}
void on_paper::Painter::paste_temp_pic(Mat pic, Point center)
{
    vector<Point2f> src,dst;
    Point2f tl,tr,bl,br;
    //src pic
    tl=Point2f(0,0);
    tr=Point2f(pic.cols,0);
    bl=Point2f(0,pic.rows);
    br=Point2f(pic.cols,pic.rows);
    src.push_back(tl);
    src.push_back(tr);
    src.push_back(bl);
    src.push_back(br);
    //dst pic
    tl=Point2f(center.x-pic.cols/2,center.y-pic.rows/2);
    tr=Point2f(center.x+pic.cols/2,center.y-pic.rows/2);
    bl=Point2f(center.x-pic.cols/2,center.y+pic.rows/2);
    br=Point2f(center.x+pic.cols/2,center.y+pic.rows/2);
    dst.push_back(tl);
    dst.push_back(tr);
    dst.push_back(bl);
    dst.push_back(br);
    //mat transfrom
    Mat transf;
    transf=getPerspectiveTransform(src,dst);

    vector<Point2f> picArray,picTrans;
    for(int i=0;i<pic.rows;i++)
    {
        for(int j=0;j<pic.cols;j++)
        {
            picArray.push_back(Point2f(i,j));
        }
    }

    utils::white_transparent(pic,pic);
    warpPerspective(pic,temp_canvas,transf,temp_canvas.size(),INTER_NEAREST);
}



void on_paper::Painter::clear_canvas(void)
{
    Mat& dst = this->canvas;
    for (int y = 0; y < dst.rows; ++y)
        for (int x = 0; x < dst.cols; ++x)
        {
            cv::Vec3b & pixel = dst.at<cv::Vec3b>(y, x);
            pixel[0]=0;
            pixel[1]=0;
            pixel[2]=0;
        }
}

const cv::Scalar &on_paper::Painter::get_color() const {
    return _color;
}

void on_paper::Painter::set_color(const cv::Scalar &_color) {
    Painter::_color = _color;
}

int on_paper::Painter::get_pen_size() const {
    return _pen_size;
}

void on_paper::Painter::set_pen_size(int _pen_size) {
    Painter::_pen_size = _pen_size;
};