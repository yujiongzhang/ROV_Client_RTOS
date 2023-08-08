#include "joystickrule.h"
#include "ui_joystickrule.h"

joyStickRule::joyStickRule(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::joyStickRule)
{
    ui->setupUi(this);
}

joyStickRule::~joyStickRule()
{
    delete ui;
}
