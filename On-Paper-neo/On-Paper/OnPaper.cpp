//
// Created by 牛天睿 on 17/5/22.
//

#include "OnPaper.h"
#include <QElapsedTimer>

void on_paper::OnPaper::camera_start()
{
    TheVideoCapturer.open(1);

    TheVideoCapturer.set(CV_CAP_PROP_FRAME_WIDTH, MAIN_WIDTH);
    TheVideoCapturer.set(CV_CAP_PROP_FRAME_HEIGHT, MAIN_HEIGHT);


    if (!TheVideoCapturer.isOpened())  throw std::runtime_error("Could not open video");

    ///// CONFIGURE DATA
    // read first image to get the dimensions
    TheVideoCapturer >> TheInputImage;
    if (TheCameraParameters.isValid())
        TheCameraParameters.resize(TheInputImage.size());

   allow_enlarge=false;
   allow_triggers=false;
   allow_write=false;

   allow_enlarge= true;
   //allow_triggers=true;
   allow_write=   true;
   last_gesture_time = utils::curtime_msec();
   last_gesture = GestureType::PRESS;

   current_page=0;
}

// must after camera_start triggered
void on_paper::OnPaper::init(){
    status=op_normal;

    TheCameraParameters.readFromXMLFile("./camera.yml");
    ac.init(TheCameraParameters);

    af.initialize();
    ac.capture_Painter(&pa);
    af.capture_Painter(&pa);
    gm = new GestureManager(&gj);
    bcd.setDecoder(BarCodeDecoder::DecoderFormat_EAN_13);
    axv.set_prefix(ARCHIVE_PREFIX);
    axv.read_storage_file(METAFILE_PATH);

}

void on_paper::OnPaper::train_hand_thrsd()
{
    bs.set_vc(&TheVideoCapturer);
    bs.train_thrsd();


    gj.set_hand_thrsd(bs.min_color, bs.max_color);
}

cv::Mat &on_paper::OnPaper::process_one_frame()
{

        //get input image!!!
    TheVideoCapturer>>TheInputImage;
    if(status == op_normal){
        return _process_normal();
    }
    else{

        warg._string="Scanning codebar ...";
        wcall("msg", warg);
        return _process_barcode();
    }

}

void on_paper::OnPaper::register_callback(string cbname, w_callback cb)
{
    this->callbacks.insert(std::make_pair(cbname, cb));
}

void on_paper::OnPaper::wcall(string cbname, __w_callback_arg arg)
{
    auto it = this->callbacks.find(cbname);
    if(it != callbacks.end()){
        it->second(arg);
    }
}


cv::Mat &on_paper::OnPaper::_process_normal()
{

    try{


        if(not ac.get_PDF_reader().is_loaded()) //no pdf is loaded.
            //TODO render"
            return TheInputImage;


        ac.input_image(TheInputImage);

        auto mknum = ac.process();//num of markers.

        if(mknum == 0) return TheInputImage;

        struct Gesture gt = gj.get_gesture(TheInputImage);
//        imshow("mask", gj.mask);
/*

        if(gt.type==GestureType::NONE)
            cout<<"NONE"<<endl;
        if(gt.type==GestureType::ENLARGE)
            cout<<"ENLARGE"<<endl;
        if(gt.type==GestureType::PRESS)
            cout<<"PRESS"<<endl;
        if(gt.type==GestureType::MOVE)
            cout<<"MOVE"<<endl;
*/
        gt.type = gm->get_uber_gesture(gt.type); // let gm to rock.

        string wmsg=gm->get_progress_message();
        //if(wmsg=="")wmsg="Place your hand here.";
        int wpercent=gm->get_progress_percentage();
        warg._string=wmsg;warg._int=wpercent;

        wcall("msg", warg);
        wcall("prog", warg);


/*
        string wmsg;
        switch(gm->get_state()){
        case NOHAND:
            wmsg="Place your hands here.";
            break;
        case INACTION:
            wmsg="In action.";
            break;
        case RECOGREADY:
            wmsg="Recognizing gesture ...";
            break;
        case ACTIONPAUSE:
            wmsg="Action paused.";
            break;
        }

        warg._string=wmsg;
        wcall("msg", warg);
*/

        auto finger_tips=vector<Point>{Point(0,0)};

        if(gt.type != GestureType::NONE)
            finger_tips = gt.fingers;

        if(current_page!=ac.get_page())
        {
            current_page=ac.get_page();
        }

        if(af.contains_triggers(current_page))
            allow_write=false;
        else
            allow_write=true;

        // let pa to rock.
        if(mknum > 0) //detected markers!
        {

            pa.with_transmatrix(ac.get_transmatrix_inv());
            //在这里可以加入对特定领域的操作
            //传入手指的位置
            af.transmatrix=ac.get_transmatrix_inv();

            if(allow_triggers) {
                Mat _pic;
                Point p;
                af.fire_event(finger_tips, p, current_page);
                if(af.get_message() != "NIL") //modified.
                    warg._string=af.get_message();
                wcall("msg", warg);
            }


            GMState current_state = gm->get_state();

            if(current_state == GMState::INACTION)
            {

                if(gt.type == GestureType::PRESS) {
                    if(last_state ==GMState::RECOGREADY)
                        pa.kalman_fix(finger_tips[0]);
                    pa.draw_finger_tips(finger_tips, pa.get_pen_size(), pa.get_color());

                    if(gm->action_gesture_changed) //changed, trace first.
                    {
                        //make kalman trace return to the finger.
                        pa.kalman_fix(finger_tips[0]);
                        if(allow_write) {
                            pa.kalman_trace(finger_tips[0], true); //trace and draw
                        }
                    } else { // just write
                        if(allow_write) {
                            pa.kalman_trace(finger_tips[0], true);
                        }
                    }
                }
                elif(gt.type == GestureType::MOVE)
                        //TODO 哪一个跟原来的点相近，我们才应该对哪个点滤波。
                {
                    pa.kalman_trace(finger_tips[0], false);
                }

                elif(gt.type == GestureType::ENLARGE)
                {
                    pa.kalman_trace(finger_tips[0], false); //follow the fingers.
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
                    if(rw > 200 and allow_enlarge){ //如果窗口太小就不管了。
                        Rect r = Rect(xsmaller, ysmaller, rw, rh);
                        pa.draw_enlarged_rect(r);
                        ac.display_enlarged_area(r);
                    }
                }
            }
            last_state = current_state;

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
        //putText(TheProcessedImage, anf, Point(0, TheProcessedImage.rows/10*7), CV_FONT_VECTOR0, 5, Scalar(0,255,0), 20,LINE_AA);

    }
        catch (std::exception &ex)
   {
        cout << "Exception :" << ex.what() << endl;
   }
    return this->TheProcessedImage;


}

cv::Mat &on_paper::OnPaper::_process_barcode()
{


    string barcode = bcd.decodeImage(utils::Mat2QImage(TheInputImage)).toStdString();
    if(barcode == "") return TheInputImage;

    archiv_conf aconf;
    barcode=barcode.substr(0, barcode.size()-1);
    //if(barcode != last_good_barcode)
    if(axv.query(barcode, aconf)) // ok ,got that, this is a good barcode.
    {
        ac.read_pdf_archiv(aconf.pdf_path);
        af.load_archiv_conf(aconf);
        this->status = op_normal;
        //this->last_good_barcode = barcode;
        warg._string="Picked up: "+aconf.name;
        wcall("msg", warg);

        if(aconf.type=="special")
            allow_enlarge=false;
        else
            allow_enlarge=true;

    }
    return TheInputImage;
}
