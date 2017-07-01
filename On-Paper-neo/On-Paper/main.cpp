#include "onpapermain.h"
#include "optestingpanel.h"
#include <QApplication>
#include "OnPaper.h"
using namespace on_paper;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //close(2);
    OnPaper op;
    op.init();
    op.camera_start();

    OnPaperMain w(&op, 0);
    OPTestingPanel p(&op, 0);
    w.show();
    p.show();
    return a.exec();
}
