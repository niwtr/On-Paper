#include "beforestart.h"

#include <unistd.h>
on_paper::BeforeStart::BeforeStart()
{
    if(HAND_MODEL)
    {
        G.register_mouseEvent_callbacks("",
                                        [&](int x, int y){
            update_thrsd(x,y);
        });
    }
}

void on_paper::BeforeStart::train_thrsd()
{
    try
    {
        *_vc >> last_img;
        *_vc >> last_img;

        resize(last_img, last_img, Size(MAIN_WIDTH, MAIN_HEIGHT));
        G.imshow(last_img);
        G.waitKey();
        G.close();
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
    cvtColor(tmp_img, tmp_img, COLOR_TO_TYPE);
    Vec3b p_color = tmp_img.at<Vec3b>(0, 0);

    min_color[0] = min(p_color[0], min_color[0]);
    min_color[1] = min(p_color[1], min_color[1]);
    min_color[2] = min(p_color[2], min_color[2]);

    max_color[0] = max(p_color[0], max_color[0]);
    max_color[1] = max(p_color[1], max_color[1]);
    max_color[2] = max(p_color[2], max_color[2]);

    if(min_color[0] == p_color[0] && min_color[0] - THRSD_UPDOWN >= 0)
        min_color[0] -= THRSD_UPDOWN;
    if(min_color[1] == p_color[1] && min_color[1] - THRSD_UPDOWN >= 0)
        min_color[1] -= THRSD_UPDOWN;
    if(min_color[2] == p_color[2] && min_color[2] - THRSD_UPDOWN >= 0)
        min_color[2] -= THRSD_UPDOWN;

    if(max_color[0] == p_color[0] && max_color[0] + THRSD_UPDOWN <= 255)
        max_color[0] += THRSD_UPDOWN;
    if(max_color[1] == p_color[1] && max_color[1] + THRSD_UPDOWN <= 255)
        max_color[1] += THRSD_UPDOWN;
    if(max_color[2] == p_color[2] && max_color[2] + THRSD_UPDOWN <= 255)
        max_color[2] += THRSD_UPDOWN;

    // draw cicle on the pixel
    circle(last_img, Point(x, y), 3, Scalar(0, 0, 255), 0);
    G.imshow(last_img);

    cout << "thre: " << min_color << "; " << max_color << endl;
}
