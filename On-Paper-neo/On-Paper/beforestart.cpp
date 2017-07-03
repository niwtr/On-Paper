#include "beforestart.h"

on_paper::BeforeStart::BeforeStart()
{
    if(HAND_MODEL)
    {
        namedWindow("Before Start");
        setMouseCallback("Before Start", mouse_trigger, this);
    }
}

void on_paper::BeforeStart::train_thrsd()
{
    try
    {
        *_vc >> last_img;
        *_vc >> last_img;

        resize(last_img, last_img, Size(MAIN_WIDTH, MAIN_HEIGHT));
        imshow("Before Start", last_img);
        waitKey(0);

        destroyWindow("Before Start");
    }
    catch (std::exception &ex)
    {
        cerr << "Exception :" << ex.what() << endl;
    }
}

void on_paper::BeforeStart::mouse_trigger(int event, int x, int y, int flags, void *vbs)
{
    if(event == CV_EVENT_LBUTTONDOWN)
    {
        BeforeStart* bs = (BeforeStart*)vbs;
        bs->update_thrsd(x, y);
    }
}

void on_paper::BeforeStart::update_thrsd(int x, int y)
{
    // get the pixel
    Vec3b p_rgb = last_img.at<Vec3b>(y, x);

    Mat tmp_img(1, 1, CV_8UC3);
    tmp_img.at<Vec3b>(0, 0) = p_rgb;
    cvtColor(tmp_img, tmp_img, CV_BGR2YCrCb);
    Vec3b p_ycrcb = tmp_img.at<Vec3b>(0, 0);

    min_ycrcb[0] = min(p_ycrcb[0], min_ycrcb[0]);
    min_ycrcb[1] = min(p_ycrcb[1], min_ycrcb[1]);
    min_ycrcb[2] = min(p_ycrcb[2], min_ycrcb[2]);

    max_ycrcb[0] = max(p_ycrcb[0], max_ycrcb[0]);
    max_ycrcb[1] = max(p_ycrcb[1], max_ycrcb[1]);
    max_ycrcb[2] = max(p_ycrcb[2], max_ycrcb[2]);

    if(min_ycrcb[0] == p_ycrcb[0])
        min_ycrcb[0] -= THRSD_UPDOWN;
    if(min_ycrcb[1] == p_ycrcb[1])
        min_ycrcb[1] -= THRSD_UPDOWN;
    if(min_ycrcb[2] == p_ycrcb[2])
        min_ycrcb[2] -= THRSD_UPDOWN;

    if(max_ycrcb[0] == p_ycrcb[0])
        max_ycrcb[0] += THRSD_UPDOWN;
    if(max_ycrcb[1] == p_ycrcb[1])
        max_ycrcb[1] += THRSD_UPDOWN;
    if(max_ycrcb[2] == p_ycrcb[2])
        max_ycrcb[2] += THRSD_UPDOWN;

    // draw cicle on the pixel
    circle(last_img, Point(x, y), 3, Scalar(0, 0, 255), 0);
    imshow("Before Start", last_img);

    cout << "thre: " << min_ycrcb << "; " << max_ycrcb << endl;
}
