#include "onpapermain.h"
#include "ui_onpapermain.h"
#include <opencv2/highgui.hpp>
OnPaperMain::OnPaperMain(OnPaper * op, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::OnPaperMain)
{
    ui->setupUi(this);
    setFixedSize(MAIN_WIDTH, MAIN_HEIGHT);
    this->centralWidget()->setFixedSize(MAIN_WIDTH, MAIN_HEIGHT);

    this->opptr = op;
    this->opptr->register_callback("msg", [this](__w_callback_arg arg){
        this->msg_broadcast(arg._string);
    });
    this->opptr->register_callback("prog", [this](__w_callback_arg arg){
        this->progbar_broadcast(arg._int);
    });
    ui->helpwidget->setStyleSheet(QString("background-color: rgba(255, 255, 255, 35%);"));
    timer_camera = new QTimer;
    timer_camera->setInterval(1000.0/fps);
    connect(this->timer_camera, SIGNAL(timeout()), this, SLOT(process_one_frame()));
    timer_camera->start();
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);
    showing=true;
    this->setWindowFlags(Qt::FramelessWindowHint);
}

void OnPaperMain::msg_broadcast(string msg)
{
    QString qmsg = QString::fromStdString(msg);
    ui->msglabel->setText(qmsg);
    //may blink here
}

void OnPaperMain::progbar_broadcast(int progress)
{
    ui->progressBar->setValue(progress);
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

void OnPaperMain::toggle_help_widget()
{
    if(showing){
        ui->helpwidget->hide();
        ui->progressBar->hide();
        ui->msglabel->hide();

    }
    else{
        ui->helpwidget->show();
        ui->progressBar->show();
        ui->msglabel->show();

    }
    showing = not showing;
}
