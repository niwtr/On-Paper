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


    try {


        cv::namedWindow("ar");
        cv::namedWindow("mask");
        //go!
        char key = 0;
        int index = 0;
        // capture until press ESC or until the end of the video

        Mat mask ; //HandDetector的mask参数，据说目前还没啥用。
        do {

            TheVideoCapturer.retrieve(TheInputImage);
//do something.
            ac.get_input_image(TheInputImage);
            ac.process();
            hd.process(TheInputImage,mask);
            ac.release_output_image(TheProcessedImage);

            cv::imshow("ar", TheProcessedImage);
            cv::imshow("mask", mask);
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

