#include "optestingpanel.h"
#include "ui_optestingpanel.h"
#include <QString>

#define SETTEXT(name)\
    ui->button_ ## name->setText(QString::fromStdString(\
    string(#name) +\
    string(": ") +\
    string(this->opptr->allow_ ## name?"ON":"OFF")))



OPTestingPanel::OPTestingPanel(OnPaper *op, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OPTestingPanel)
{
    ui->setupUi(this);
    this->opptr = op;
    ui->button_antishake->setText("Antishake");
    ui->button_status_norm->setText("Normal Mode");
    ui->button_status_barcode->setText("Barcode Mode");
    SETTEXT(triggers);
    SETTEXT(enlarge);
    SETTEXT(write);

}

OPTestingPanel::~OPTestingPanel()
{
    delete ui;
}
#define TOGGLE_FUNCTION(name)\
    this->opptr->allow_ ## name = not this->opptr->allow_ ## name
void OPTestingPanel::on_button_write_clicked()
{
    TOGGLE_FUNCTION(write);
    SETTEXT(write);
}

void OPTestingPanel::on_button_enlarge_clicked()
{
    TOGGLE_FUNCTION(enlarge);
    SETTEXT(enlarge);
}

void OPTestingPanel::on_button_triggers_clicked()
{
    TOGGLE_FUNCTION(triggers);
    SETTEXT(triggers);
}

void OPTestingPanel::on_button_antishake_clicked()
{
    opptr->ac.toggle_anti_shake();
}


#undef TOGGLE_FUNCTION
#undef SETTEXT

void OPTestingPanel::on_button_status_norm_clicked()
{
    opptr->status=op_status::op_normal;
}

void OPTestingPanel::on_button_status_barcode_clicked()
{
    opptr->status=op_status::op_barcode;
}
