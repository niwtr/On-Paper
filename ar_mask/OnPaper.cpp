//
// Created by 牛天睿 on 17/5/22.
//

#include "OnPaper.h"

void on_paper::OnPaper::main_loop(void) {

    TheVideoCapturer.open(0);

    TheVideoCapturer.set(CV_CAP_PROP_FRAME_WIDTH, 800);
    TheVideoCapturer.set(CV_CAP_PROP_FRAME_HEIGHT, 600);

    int waitTime=1;
    if (!TheVideoCapturer.isOpened())  throw std::runtime_error("Could not open video");

    ///// CONFIGURE DATA
    // read first image to get the dimensions
    TheVideoCapturer >> TheInputImage;
    if (TheCameraParameters.isValid())
        TheCameraParameters.resize(TheInputImage.size());

    //this->pa.init(TheInputImage.rows, TheInputImage.cols);

    tb.init(TheInputImage.rows, TheInputImage.cols);


    Scalar line_color = Scalar(255, 255, 0);


    try {


        cv::namedWindow("ar");
        //cv::namedWindow("mask");
        //go!
        char key = 0;
        int index = 0;
        // capture until press ESC or until the end of the video

        Mat mask ; //HandDetector的mask参数，据说目前还没啥用。
        int page=0;
        do {

            TheVideoCapturer.retrieve(TheInputImage);
            //do something.
            ac.input_image(TheInputImage);
            auto mknum = ac.process();//num of markers.
            struct Gesture gt = gj.get_gesture(TheInputImage);
            mask = gj.mask;
            auto finger_tips=vector<Point>{Point(0,0)};

            if(gt.type != GestureType::NONE)
                finger_tips = gt.fingers;

            if(page!=ac.get_page())
            {
                page=ac.get_page();
                af.init(page);
            }

            // let pa to rock.
            if(mknum > 0) //detected markers!
            {
                //tb.fire_event(finger_tip);
                pa.with_transmatrix(ac.get_transmatrix_inv());
                //在这里可以加入对特定领域的操作
                //传入手指的位置
                af.transmatrix=ac.get_transmatrix_inv();
                Mat _pic;
                Point p;
                af.showPic(finger_tips,p);
                if(p!=Point(0,0))
                {
                    _pic=af.get_pic();
                    pa.pastePic(_pic,p);
                }
                if(gt.type == GestureType::PRESS)
                    pa.kalman_trace(finger_tips[0], 5, line_color, true);
                elif(gt.type == GestureType::MOVE)
                    //TODO 哪一个跟原来的点相近，我们才应该对哪个点滤波。
                    pa.kalman_trace(finger_tips[0], 5, line_color, false);
                    //an ad-hoc solution.
                elif(gt.type == GestureType::ENLARGE)
                {
                    pa.kalman_trace(finger_tips[0], 5, line_color, false);
                    Mat _image = ac.get_image();
                    vector<Point> vp = finger_tips;
                    for(auto & p : vp)
                        pa.transform_point(p);
                    auto p1 = vp[0], p2=vp[1];
                    ac.adjust_point(p1);
                    ac.adjust_point(p2);
                    int xsmaller = p1.x<p2.x? p1.x:p2.x;
                    int ysmaller = p1.y<p2.y? p1.y:p2.y;
                    auto rw = abs(p1.x-p2.x), rh = (int)((float)rw/640*480);
                    if(rw > 0 or rh > 0) {
                        Rect r = Rect(xsmaller, ysmaller, rw, rh);
                        pa.draw_enlarged_rect(r);
                        ac.display_enlarged_area(r);
                    }
                }
                pa.transform_canvas(ac.get_transmatrix(), TheInputImage.size());
            }

            //Overlay!

            lm.capture(ac.get_processed_image());
            if(mknum>0) {
                lm.capture(ac.get_virtual_paper_layer());
                lm.capture(pa.get_canvas_layer());
                lm.capture(pa.get_temp_canvas_layer());
            }

            lm.overlay();
            lm.output(TheProcessedImage);
            if(gt.type!=GestureType::NONE)
                for(const auto& finger_tip : finger_tips)
                circle(TheProcessedImage, finger_tip, 4, Scalar(0, 0, 255), 4);


            cv::imshow("ar", TheProcessedImage);
            //cv::imshow("mask", mask);

            key = (char)cv::waitKey(1); // wait for key to be pressed
            if(key=='s')  {
                waitTime= waitTime==0?1:0;
            } else if(key == 'a')
                ac.toggle_anti_shake();

            index++; // number of images captured

        } while (key != 27 && (TheVideoCapturer.grab() ));

    } catch (std::exception &ex)
    {
        cout << "Exception :" << ex.what() << endl;
    }

}

