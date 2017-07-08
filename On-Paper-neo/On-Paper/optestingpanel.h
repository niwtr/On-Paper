#ifndef OPTESTINGPANEL_H
#define OPTESTINGPANEL_H

#include <QWidget>
#include "OnPaper.h"
namespace Ui {
class OPTestingPanel;
}
using namespace  on_paper;
class OPTestingPanel : public QWidget
{
    Q_OBJECT

public:
    explicit OPTestingPanel(OnPaper * op, QWidget *parent = 0);
    ~OPTestingPanel();

private slots:
    void on_button_write_clicked();

    void on_button_enlarge_clicked();

    void on_button_triggers_clicked();

    void on_button_antishake_clicked();

    void on_button_status_norm_clicked();

    void on_button_status_barcode_clicked();

private:
    Ui::OPTestingPanel *ui;
    OnPaper * opptr;

};

#endif // OPTESTINGPANEL_H
