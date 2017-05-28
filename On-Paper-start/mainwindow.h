#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>
//#include "QZXing"

#include "opencv2/opencv.hpp"
#include "aruco/aruco.h"

using namespace cv;
using namespace std;
using namespace aruco;

#define FPS 30

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void startCap();

    QImage MatToQImage(const cv::Mat& mat);

private slots:
    void nextFrame();

private:
    Ui::MainWindow *ui;

    CameraParameters cam_param;
    MarkerDetector m_detector;
//    QZXing qr_decoder;
    VideoCapture cap;
    QTimer *timer;
};

#endif // MAINWINDOW_H
