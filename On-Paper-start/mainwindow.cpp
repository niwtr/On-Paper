#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "opencv2/opencv.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(this->width(), this->height());
}

MainWindow::~MainWindow()
{
    delete timer;
    delete ui;
}

void MainWindow::startCap()
{
    cam_param.readFromXMLFile("camera.yml");
    m_detector.setDictionary("ARUCO");
    m_detector.setThresholdParams(7, 7);
    m_detector.setThresholdParamRange(2, 0);
//    qr_decoder.setDecoder(QZXing::DecoderFormat_QR_CODE);

    cap.open(0);
    if(!cap.isOpened())
    {
        cerr << "Can`t open videocapture" << endl;
        exit(1);
    }

    timer = new QTimer(this);
    timer->setInterval(1000.0/FPS);   //set timer match with FPS
    connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
    timer->start();
}

void MainWindow::nextFrame()
{
    Mat img;
    QImage qimg;
    vector<Marker> markers;

    cap >> img;
    if(img.empty())
    {
        cerr << "VideoCapture read error" << endl;
        exit(1);
    }
//    QString result = qr_decoder.decodeImage(qimg);

    float marker_sz = 0.0565;
    markers = m_detector.detect(img, cam_param, marker_sz);

    if(markers.size() > 0)
        putText(img, "Got it: <<On-Paper>>", Point(20, 50), CV_FONT_HERSHEY_COMPLEX|CV_FONT_ITALIC, 1, Scalar(0, 255, 0), 2);
    else
        putText(img, "Scanning...", Point(20, 50), CV_FONT_HERSHEY_COMPLEX|CV_FONT_ITALIC, 1, Scalar(255, 0, 0), 2);

    qimg = MatToQImage(img);
    ui->image_label->setPixmap(QPixmap::fromImage(qimg).scaled(ui->image_label->size()));

    if(markers.size() > 0)
    {
        QMessageBox::information(this, QString("Got it"),QString("A new world is opening..."));
        this->close();
    }
}

QImage MainWindow::MatToQImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}
