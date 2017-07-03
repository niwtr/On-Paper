#ifndef BEFORESTART_H
#define BEFORESTART_H

#include "opencv2/opencv.hpp"
#include "defs.h"

#include <iostream>

namespace on_paper {

using namespace std;
using namespace cv;

class BeforeStart
{
public:
    BeforeStart();
    void set_vc(VideoCapture *vc)
    {
        this->_vc = vc;
    }

    void train_thrsd();
    void update_thrsd(int x, int y);

    static void mouse_trigger(int event, int x, int y, int flags, void *vbs);

    Vec3b min_color = Vec3b(255, 255, 255);
    Vec3b max_color = Vec3b(0, 0, 0);

private:
    VideoCapture *_vc;
    Mat last_img;
};
}


#endif // BEFORESTART_H
