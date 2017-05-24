//
// Created by 牛天睿 on 17/5/18.
//

#include "ARCapturer.h"



// must call get_input_image first!
unsigned long on_paper::ARCapturer::process() {
    // copy image
    // Detection of markers in the image passed
    TheMarkers= MDetector.detect(TheInputImageCopy, CamParams, TheMarkerSize);
    fill_markers();
    if(perform_anti_shake)
        anti_shake();
    map_markers();
    TheLastMarkers=TheMarkers;
    return TheMarkers.size();
}

void on_paper::ARCapturer::init(CameraParameters cp) {

    this->CamParams = cp;
    MDetector.setCornerRefinementMethod(aruco::MarkerDetector::SUBPIX);
    TheInputImageCopy = Mat::zeros(100,100,CV_8UC1); //FIXME potential bug.
    TheLastMarkers=MDetector.detect(TheInputImageCopy, CamParams, TheMarkerSize);
    image = imread(string(ROOT) + "x-0.png");
    //image= readPDFtoCV("../rt.pdf", 300);
    cvtColor(image, image, CV_BGR2BGRA);
    // read camera parameters if passed


    auto&& get_shifted_paper_pattern=[](float ratio_x, float ratio_y, const vector<Point2f> vp2f){
        assert(vp2f.size()==4);
        float height = vp2f[0].y*2;
        float width  = vp2f[0].x*2;
        float shifty = (height*ratio_y),
                shiftx= (width*ratio_x);

        vector<Point2f> ps ={
                Point2f(vp2f[0].x+shiftx, vp2f[0].y+shifty),
                Point2f(vp2f[1].x+shiftx, vp2f[1].y+shifty),
                Point2f(vp2f[2].x+shiftx, vp2f[2].y+shifty),
                Point2f(vp2f[3].x+shiftx, vp2f[3].y+shifty)
        };
        return std::move(ps);

    };
    //add shifted patterns.
    for(auto& s : shifts)
        this->shifted_pattern_paper_source.emplace_back(
                get_shifted_paper_pattern(s.first, s.second, pattern_paper_source));

    this->original_image_pattern = {
            Point2f(image.cols, image.rows),
            Point2f(0, image.rows),
            Point2f(0, 0),
            Point2f(image.cols, 0)
    };
    MDetector.setDictionary("ARUCO");
    MDetector.setThresholdParams(7, 7);
    MDetector.setThresholdParamRange(2, 0);
    this->perform_anti_shake = false;

}


#define S2(X) ((X)*(X))
template<typename vecpf>
float on_paper::ARCapturer::euclid_dist(const vecpf &v1, const vecpf &v2) {
    if(v1.size()!=v2.size())return -1;
    float acc=0;
    for(auto i = 0; i< v1.size();i++){
        auto& p1=v1[i];
        auto& p2=v2[i];
        acc+= sqrt(S2(p1.x-p2.x)+S2(p1.y-p2.y));
    }
    return acc/v1.size();
}
#undef S2


void on_paper::ARCapturer::white_transparent(const cv::Mat &src, cv::Mat &dst) {
    cv::cvtColor(src, dst, CV_BGR2BGRA);
    // find all white pixel and set alpha value to zero:
    for (int y = 0; y < dst.rows; ++y)
        for (int x = 0; x < dst.cols; ++x)
        {
            cv::Vec4b & pixel = dst.at<cv::Vec4b>(y, x);
            // if pixel is white
            if (pixel[0] > 250 && pixel[1] > 250 && pixel[2] > 250)
            {
                // set alpha to zero:
                pixel[3] = 0;
            }
        }
}

cv::Mat on_paper::ARCapturer::resize(const cv::Mat &in, int width)
{
        if (in.size().width<=width) return in;
        float yf=float(  width)/float(in.size().width);
        cv::Mat im2;
        cv::resize(in,im2,cv::Size(width,float(in.size().height)*yf));
        return im2;
}



/**
 * @brief 把所得的marker映射到纸上。
 * 将会把marker映射到的四个点进行平均。
 * 遵循以下策略：
 * 1. 如果没有中间部分的marker，使用两个角上的任意一个marker。
 * 2. 如果存在中间部分的marker，使用两个marker映射结果的平均值。(TODO 加上两个角上的marker的干预)
 */
void on_paper::ARCapturer::map_markers(void) {

    bool exists_in_middle = false;
    if (TheMarkers.size() > 0 && CamParams.isValid() && TheMarkerSize > 0)
    {
        for(auto& m : TheMarkers){
            if(m.id%4==1 or m.id%4 ==2)//中间部分的marker
            exists_in_middle = true;
        }

    } else return;//do nothing and back!

    vector<Point2f> virtual_paper ;

    for (unsigned long num = 0; num < TheMarkers.size(); num++) {

        Marker &lamaker = TheMarkers[num];
        //如果存在中间点，则忽略两侧的点。
        if(exists_in_middle and (lamaker.id%4 == 0 or lamaker.id%4 == 3))
            continue;

        Point mcenter = lamaker.getCenter();
        if (lamaker.size() != 4) {
            std::cout << "Panic! " << std::endl;
            exit(-9);
        }
        Mat M0=getPerspectiveTransform(pattern_marker_source, lamaker);

        vector<Point2f> __virtual_paper;
        perspectiveTransform(shifted_pattern_paper_source[lamaker.id%4], __virtual_paper, M0);

        //circle(TheInputImageCopy, mcenter, 10, Scalar(0,255,0),10);
        //for(int ii=0;ii< 4;ii++)
//                    circle(TheInputImageCopy, lamaker[ii], 10, Scalar(0, ii*50, 0), 10);

        if(virtual_paper.size()==0)
            virtual_paper=__virtual_paper;
        else
            //如果我们有两个marker，用它进行平均。
            virtual_paper = vector_avg2(virtual_paper, __virtual_paper);
    }

    //把原图投射到虚拟纸张上。
    Mat M = getPerspectiveTransform(original_image_pattern, virtual_paper);
    Mat M_inv = getPerspectiveTransform(virtual_paper, original_image_pattern);
    this->transmatrix_inv = M_inv; //set val to M_inv.
    this->transmatrix = M; // set val to M.
    Mat transf = Mat::zeros(TheInputImageCopy.size(), CV_8UC4);
    warpPerspective(image, transf, M, TheInputImageCopy.size(), cv::INTER_NEAREST);
    white_transparent(transf, transf);

    VirtualPaperImage = transf;

    //Mat output;
    //overlayImage(TheInputImageCopy, transf, TheInputImageCopy, Point(0, 0));
}

vector<cv::Point2f> on_paper::ARCapturer::vector_avg2(const vector<cv::Point2f> &src1, const vector<cv::Point2f> &src2)
{
    vector<Point2f> dst;
    if(src1.size()!= src2.size()){
        cout<<"Vectors must be of same size!"<<endl;
        exit(999);
    }
    for(auto i = 0;i<src1.size();i++)
        dst.push_back(Point2f((src1[i].x+src2[i].x)/2, (src1[i].y+src2[i].y)/2));
    return dst;
}

void on_paper::ARCapturer::anti_shake(void) {
#define distance(a,b) (sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)))
    for (unsigned int i = 0; i < TheMarkers.size()&&i<TheLastMarkers.size(); i++) {
        Point center=TheMarkers[i].getCenter();
        Point pre_center=TheLastMarkers[i].getCenter();
        if(distance(center,pre_center)<10)
            TheMarkers[i]=TheLastMarkers[i];
    }
}

void on_paper::ARCapturer::fill_markers(void) {
    for(auto& lamaker : TheMarkers) {//fill the marker with white.
        vector<Point> corners(lamaker.begin(), lamaker.end());
        vector<vector<Point>> c_corners = {corners};
        fillPoly(TheInputImageCopy, c_corners, Scalar(180, 180, 180));
    }
}

//void on_paper::ARCapturer::overlayCanvas(const cv::Mat &canvas) {
//    if(transmatrix.empty())//当transmatrix为空的时候可不要overlay。这是初始情况
//        //TODO 尽量避免初始情况。使用更优雅的初始化函数。
//        return;
//    //TODO 尽量减少cvtColor?
//    cvtColor(TheInputImageCopy, TheInputImageCopy, CV_BGRA2BGR);
//    Mat transf = Mat::zeros(TheInputImageCopy.size(), CV_8UC3);
//    warpPerspective(canvas, transf, this->transmatrix, TheInputImageCopy.size(), cv::INTER_NEAREST);
//    cv::addWeighted(transf, 1, TheInputImageCopy, 1, 0, TheInputImageCopy);
//
//}

#undef distance

