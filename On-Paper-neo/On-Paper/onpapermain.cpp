#include "onpapermain.h"
#include "ui_onpapermain.h"
#include <opencv2/highgui.hpp>
OnPaperMain::OnPaperMain(OnPaper * op, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::OnPaperMain)
{
    ui->setupUi(this);
    setFixedSize(MAIN_WIDTH, MAIN_HEIGHT);

    this->opptr = op;
    timer_camera = new QTimer;
    timer_camera->setInterval(1000.0/fps);
    connect(this->timer_camera, SIGNAL(timeout()), this, SLOT(process_one_frame()));

    timer_camera->start();

}

OnPaperMain::~OnPaperMain()
{
    delete ui;
}

void OnPaperMain::process_one_frame()
{
    Mat processed=opptr->process_one_frame();
    ui->gl_canv->setImage(utils::Mat2QImage(processed));
    ui->gl_canv->update();
}
