//
// Created by 牛天睿 on 17/5/22.
//

#include "OnPaper.h"
#include <ctime>


void on_paper::OnPaper::camera_start()
{
    TheVideoCapturer.open(0);

    TheVideoCapturer.set(CV_CAP_PROP_FRAME_WIDTH, 1024);
    TheVideoCapturer.set(CV_CAP_PROP_FRAME_HEIGHT, 768);


    if (!TheVideoCapturer.isOpened())  throw std::runtime_error("Could not open video");

    ///// CONFIGURE DATA
    // read first image to get the dimensions
    TheVideoCapturer >> TheInputImage;
    if (TheCameraParameters.isValid())
        TheCameraParameters.resize(TheInputImage.size());


   allow_enlarge=false;
   allow_triggers=false;
   allow_write=false;
   last_gesture_time = utils::curtime_msec();
   last_gesture = GestureType::PRESS;

   current_page=0;
}


cv::Mat &on_paper::OnPaper::process_one_frame()
{

    try{

        //get input image!!!
        TheVideoCapturer>>TheInputImage;

        ac.input_image(TheInputImage);
        auto mknum = ac.process();//num of markers.

        struct Gesture gt = gj.get_gesture(TheInputImage);

        if(gt.type==GestureType::NONE)
            cout<<"NONE"<<endl;
        if(gt.type==GestureType::ENLARGE)
            cout<<"ENLARGE"<<endl;
        if(gt.type==GestureType::PRESS)
            cout<<"PRESS"<<endl;
        if(gt.type==GestureType::MOVE)
            cout<<"MOVE"<<endl;

        auto finger_tips=vector<Point>{Point(0,0)};

        if(gt.type != GestureType::NONE)
            finger_tips = gt.fingers;

        if(current_page!=ac.get_page())
        {
            current_page=ac.get_page();
            af.init(current_page);
        }

        // let pa to rock.
        if(mknum > 0) //detected markers!
        {
            //tb.fire_event(finger_tip);
            pa.with_transmatrix(ac.get_transmatrix_inv());
            //在这里可以加入对特定领域的操作
            //传入手指的位置
            af.transmatrix=ac.get_transmatrix_inv();

            if(allow_triggers) {
                Mat _pic;
                Point p;
                af.fire_event(finger_tips, p);
            }


            if(gt.type == GestureType::PRESS) {
                pa.draw_finger_tips(finger_tips, pa.get_pen_size(), pa.get_color());

                auto curtime = utils::curtime_msec();
                if(last_gesture == GestureType::MOVE or last_gesture == GestureType::ENLARGE) {
                    //之前不是press手势，说明这是从其他手势恢复到划线手势。这时，需要等待400毫秒
                    //等待400毫秒是为了让用户有足够的时间准备好划线。
                    //因为用户手势改变的动作是会占用时间的。
                    if(curtime - last_gesture_time > 400)//timeout for 400msec
                    {
                        //进行三十次卡尔曼追踪使得划线的光标回归手指。
                        //注意30次只是经验。
                        for(int i = 0;i<30;i++)
                            pa.kalman_trace(finger_tips[0], false);
                        last_gesture_time = curtime;
                        last_gesture = PRESS;
                        if(allow_write) {
                            pa.kalman_trace(finger_tips[0], true); //trace and draw
                            pa.text_broadcast("Writing.");
                        }
                    }

                } else {
                    last_gesture_time = curtime;
                    last_gesture = PRESS;
                    if(allow_write) {
                        pa.kalman_trace(finger_tips[0], true);
                        pa.text_broadcast("Writing.");
                    }
                }

            }
            elif(gt.type == GestureType::MOVE)
                    //TODO 哪一个跟原来的点相近，我们才应该对哪个点滤波。
            {
                pa.kalman_trace(finger_tips[0], false);
                last_gesture = GestureType::MOVE;
                last_gesture_time=utils::curtime_msec();
            }
            //an ad-hoc solution.
            elif(gt.type == GestureType::ENLARGE)
            {
                if(last_gesture == GestureType::PRESS){
                    auto thistime = utils::curtime_msec();
                    if(thistime-last_gesture_time<300)
                        goto next; // PRESS affinity
                }
                last_gesture = GestureType::ENLARGE;
                last_gesture_time=utils::curtime_msec();
                pa.kalman_trace(finger_tips[0], false);
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
                if(rw > 500 and allow_enlarge){ //如果窗口太小就不管了。
                    Rect r = Rect(xsmaller, ysmaller, rw, rh);
                    pa.draw_enlarged_rect(r);
                    ac.display_enlarged_area(r);
                    pa.text_broadcast("Enlarge.");
                }
            }

            pa.transform_canvas(ac.get_transmatrix(), TheInputImage.size());
        }


next:
        //Overlay!

        lm.capture(ac.get_processed_image());
        if(mknum>0) {
            lm.capture(ac.get_virtual_paper_layer());
            lm.capture(pa.get_canvas_layer());
            lm.capture(pa.get_temp_canvas_layer());
        }

        lm.overlay();
        lm.output(TheProcessedImage);


    }catch (std::exception &ex)
    {
        cout << "Exception :" << ex.what() << endl;
    }
    return this->TheProcessedImage;

}

