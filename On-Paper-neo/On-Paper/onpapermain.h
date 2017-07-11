#ifndef ONPAPERMAIN_H
#define ONPAPERMAIN_H

#include <QMainWindow>
#include <QTimer>
#include "glcanvas.h"
#include "OnPaper.h"
#include <string>


namespace Ui {
class OnPaperMain;
}
using namespace on_paper;
class OnPaperMain : public QMainWindow
{

    Q_OBJECT

public:
    explicit OnPaperMain(OnPaper* op, QWidget *parent = 0);

    void msg_broadcast(std::string msg);
    void progbar_broadcast(int progress);

    void toggle_help_widget();
    ~OnPaperMain();
private slots:
    void process_one_frame();




private:
    int fps=60;
    bool showing;
    Ui::OnPaperMain *ui;
    QTimer * timer_camera;

    OnPaper* opptr;
};

#endif // ONPAPERMAIN_H
