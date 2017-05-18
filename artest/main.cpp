
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "aruco/aruco.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-page-renderer.h>
#include <poppler/cpp/poppler-image.h>
#include <cmath>
#include <opencv/cv.hpp>



using namespace cv;
using namespace aruco;

MarkerDetector MDetector;
VideoCapture TheVideoCapturer;
vector< Marker > TheMarkers;
Mat TheInputImage, TheInputImageCopy;
CameraParameters TheCameraParameters;

Mat image;
float a4_width = 0.210;
float a4_height = 0.297;


cv::Mat resize(const cv::Mat &in,int width){
    if (in.size().width<=width) return in;
    float yf=float(  width)/float(in.size().width);
    cv::Mat im2;
    cv::resize(in,im2,cv::Size(width,float(in.size().height)*yf));
    return im2;

}


Mat readPDFtoCV(const string& filename,int DPI) {
    poppler::document* mypdf = poppler::document::load_from_file(filename);
    if(mypdf == NULL) {
        cerr << "couldn't read pdf\n";
        return Mat();
    }
    cout << "pdf has " << mypdf->pages() << " pages\n";
    poppler::page* mypage = mypdf->create_page(0);

    poppler::page_renderer renderer;
    renderer.set_render_hint(poppler::page_renderer::text_antialiasing);
    poppler::image myimage = renderer.render_page(mypage,DPI,DPI);
    cout << "created image of  " << myimage.width() << "x"<< myimage.height() << "\n";

    Mat cvimg;
    if(myimage.format() == poppler::image::format_rgb24) {
        Mat(myimage.height(),myimage.width(),CV_8UC3,myimage.data()).copyTo(cvimg);
    } else if(myimage.format() == poppler::image::format_argb32) {
        Mat(myimage.height(),myimage.width(),CV_8UC4,myimage.data()).copyTo(cvimg);
    } else {
        cerr << "PDF format no good\n";
        return Mat();
    }
    return cvimg;
}


void scaleROI(vector<Point2f> & v,
              const Point2f& center, float scalex, float scaley){
    Point2f &tl=v[0],&tr=v[1],&br=v[2],&bl=v[3];

    //TopLeft
    float shiftX = abs(center.x -tl.x);
    float shiftY = abs(center.y -tl.y);

    shiftX = shiftX * scalex;
    shiftY = shiftY * scaley;

    tl.x = center.x - shiftX;
    tl.y = center.y - shiftY;


    //topright

    shiftX = abs(tr.x - center.x);
    shiftY = abs(tr.y - center.y);

    shiftX = shiftX * scalex;
    shiftY = shiftY * scaley;

    tr.x = center.x + shiftX;
    tr.y = center.y - shiftY;

    //> BottomRight

    shiftX = abs(br.x - center.x);
    shiftY = abs(br.y - center.y);

    shiftX = shiftX * scalex;
    shiftY = shiftY * scaley;

    br.x = center.x + shiftX;
    br.y = center.y + shiftY;
    //BottomLeft
    shiftX = abs(center.x - bl.x);
    shiftY = abs(bl.y - center.y);

    shiftX = shiftX * scalex;
    shiftY = shiftY * scaley;

    bl.x = center.x - shiftX;
    bl.y = center.y + shiftY;
}

void white_transparent(const Mat & src, Mat& dst){
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
void overlayImage(const cv::Mat &background, const cv::Mat &foreground,
                  cv::Mat &output, cv::Point2i location)
{
    background.copyTo(output);


    // start at the row indicated by location, or at row 0 if location.y is negative.
    for(int y = std::max(location.y , 0); y < background.rows; ++y)
    {
        int fY = y - location.y; // because of the translation

        // we are done of we have processed all rows of the foreground image.
        if(fY >= foreground.rows)
            break;

        // start at the column indicated by location,

        // or at column 0 if location.x is negative.
        for(int x = std::max(location.x, 0); x < background.cols; ++x)
        {
            int fX = x - location.x; // because of the translation.

            // we are done with this row if the column is outside of the foreground image.
            if(fX >= foreground.cols)
                break;

            // determine the opacity of the foregrond pixel, using its fourth (alpha) channel.
            double opacity =
                    ((double)foreground.data[fY * foreground.step + fX * foreground.channels() + 3])

                    / 255.;


            // and now combine the background and foreground pixel, using the opacity,

            // but only if opacity > 0.
            for(int c = 0; opacity > 0 && c < output.channels(); ++c)
            {
                unsigned char foregroundPx =
                        foreground.data[fY * foreground.step + fX * foreground.channels() + c];
                unsigned char backgroundPx =
                        background.data[y * background.step + x * background.channels() + c];
                output.data[y*output.step + output.channels()*x + c] =
                        backgroundPx * (1.-opacity) + foregroundPx * opacity;
            }
        }
    }
}

//左上角的Marker: -0.36548, -0.40488
//左下角的Marker：-0.36548, 0.40488
//中上的Marker: 0， -0.16667
//中下的Marker: 0，0.16667
/**
 * @brief 获取经过平移变换之后的pattern。
 * 这是因为我们的Marker不在纸张的正中央。需要对纸张的原始pattern进行变换。
 * @param ratio_x 水平方向平移的比率（左负右正）
 * @param ratio_y 竖直平移的比率（上负下正）
 * @param image 原图像
 * @return 经过平移之后的pattern
 */
auto&& get_translation_pattern_s=[](float ratio_x, float ratio_y, const Mat& image) {
    float rows= image.rows,
            cols=image.cols;
    float shifty = (rows*ratio_y),
            shiftx=(cols*ratio_x);

    vector<Point2f> ps ={
            Point2f(cols+shiftx, rows+shifty),
            Point2f(shiftx, rows+shifty),
            Point2f(shiftx, shifty),
            Point2f(cols+shiftx, shifty)
    };
    return std::move(ps);
};
#define S2(X) ((X)*(X))
template<typename vecpf>
float euclid_dist(const vecpf& v1, const vecpf& v2){
    if(v1.size()!=v2.size())return -1;
    float acc=0;
    for(auto i = 0; i< v1.size();i++){
        auto& p1=v1[i];
        auto& p2=v2[i];
        acc+= sqrt(S2(p1.x-p2.x)+S2(p1.y-p2.y));
    }
    return acc/v1.size();
}


int main(int argc, char **argv) {

    try {

        image = imread("../x-0.png");
        //image= readPDFtoCV("../rt.pdf", 300);
        cvtColor(image, image, CV_BGR2BGRA);
        // read camera parameters if passed
        TheCameraParameters.readFromXMLFile("../camera.yml");
        float TheMarkerSize = 0.0290;//0.0565;
        float len_mark = sqrt(TheMarkerSize*TheMarkerSize+TheMarkerSize*TheMarkerSize);
        float len_a4 = sqrt(a4_height*a4_height+a4_width*a4_width);

        auto&& pattern_image_s = get_translation_pattern_s(0, -0.16667, image);
        const vector<Point2f> pattern_marker_s = {
                Point2f(-1,1),
                Point2f(1, 1),
                Point2f(1, -1),
                Point2f(-1, -1)
        };
        vector<Point2f> pattern_paper_s = vector<Point2f>(pattern_marker_s.begin(), pattern_marker_s.end());

        for (auto & i : pattern_paper_s){
            i.x*=a4_width/TheMarkerSize;
            i.y*=a4_height/TheMarkerSize;
        }

        Marker last_marker;


        TheVideoCapturer.open(0);


        int waitTime=1;
        if (!TheVideoCapturer.isOpened())  throw std::runtime_error("Could not open video");

        ///// CONFIGURE DATA
        // read first image to get the dimensions
        TheVideoCapturer >> TheInputImage;
        if (TheCameraParameters.isValid())
            TheCameraParameters.resize(TheInputImage.size());

        MDetector.setDictionary("ARUCO");
        MDetector.setThresholdParams(7, 7);
        MDetector.setThresholdParamRange(2, 0);
        //  MDetector.setCornerRefinementMethod(aruco::MarkerDetector::SUBPIX);

        //gui requirements : the trackbars to change this parameters
        cv::namedWindow("in");

        //go!
        char key = 0;
        int index = 0;
        // capture until press ESC or until the end of the video
        do {

            TheVideoCapturer.retrieve(TheInputImage);
            // copy image
            // Detection of markers in the image passed
            TheMarkers= MDetector.detect(TheInputImage, TheCameraParameters, TheMarkerSize);

            // print marker info and draw the markers in image
            TheInputImage.copyTo(TheInputImageCopy);


            if (TheMarkers.size() > 0 && TheCameraParameters.isValid() && TheMarkerSize > 0)
                for (unsigned int i = 0; i < TheMarkers.size(); i++) {


                    Marker &lamaker = TheMarkers[i];
                    //last_marker=lamarker;

                    if (last_marker.size() == 0)
                        last_marker = lamaker;

//                    if(euclid_dist(last_marker, lamaker)<200)
//                        lamaker=last_marker;


                    Point mcenter = lamaker.getCenter();

                    if (lamaker.size() == 4) {
                        vector<Point> corners(lamaker.begin(), lamaker.end());
                        vector<vector<Point>> c_corners = {corners};
                        fillPoly(TheInputImageCopy, c_corners, Scalar(180,180,180));
                        // marker calib -> real  marker
                        Mat M0=getPerspectiveTransform(pattern_marker_s, lamaker);

                        //the virtual a4 paper.
                        vector<Point2f> pattern_paper_v;
                        perspectiveTransform(pattern_paper_s, pattern_paper_v, M0);

                        circle(TheInputImageCopy, mcenter, 10, Scalar(0,255,0),10);
                        for(auto ii : lamaker)
                            circle(TheInputImageCopy, ii, 10, Scalar(0,255,0), 10);

                        /* 一旦我们有了虚拟a4纸的四个点，用平移之后的原图的四个点计算变换阵M，
                         * 则如果我们拿原图的四个点，用变换阵M做变换，
                         * 就得到了平移后的a4纸上对应的四个点。*/
                        /* you are not expected to understand this. */
                        Mat M = getPerspectiveTransform(pattern_image_s, pattern_paper_v);
                        Mat transf = Mat::zeros(TheInputImageCopy.size(), CV_8UC4);
                        warpPerspective(image, transf, M, TheInputImageCopy.size(), INTER_NEAREST);
                        white_transparent(transf, transf);
                        //Mat output;
                        overlayImage(TheInputImageCopy, transf, TheInputImageCopy, Point(0, 0));
                    }
                }

            cv::imshow("in", resize(TheInputImageCopy,640));

            key = (char)cv::waitKey(1); // wait for key to be pressed
            if(key=='s')  waitTime= waitTime==0?1:0;
            index++; // number of images captured
        } while (key != 27 && (TheVideoCapturer.grab() ));

    } catch (std::exception &ex)

    {
        cout << "Exception :" << ex.what() << endl;
    }
}
