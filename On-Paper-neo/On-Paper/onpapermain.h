#ifndef ONPAPERMAIN_H
#define ONPAPERMAIN_H

#include <QMainWindow>
#include <QTimer>
#include "glcanvas.h"
#include "OnPaper.h"

namespace Ui {
class OnPaperMain;
}
using namespace on_paper;
class OnPaperMain : public QMainWindow
{

    Q_OBJECT

public:
    explicit OnPaperMain(OnPaper* op, QWidget *parent = 0);
    ~OnPaperMain();
private slots:
    void process_one_frame();



private:
    int fps=60;
    Ui::OnPaperMain *ui;
    QTimer * timer_camera;

    OnPaper* opptr;
};

#endif // ONPAPERMAIN_H
