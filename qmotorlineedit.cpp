#include "qmotorlineedit.h"

qMotorLineEdit::qMotorLineEdit(QWidget *parent) : QLineEdit(parent)
{

}

void qMotorLineEdit::updatemotor(float motor_speed)
{
    if(motor_speed > 0)
    {
        this->setStyleSheet("background-color: rgb(85, 170, 255)");
        this->setText(QString::number(motor_speed));
    }
    else if(motor_speed < 0)
    {
        this->setStyleSheet("background-color: rgb(150, 255, 0)");
        this->setText(QString::number(motor_speed));
    }
    else if(motor_speed == 0)
    {
        this->setStyleSheet("background-color: rgb(255, 210, 0)");
        this->setText(QString::number(motor_speed));
    }


}
