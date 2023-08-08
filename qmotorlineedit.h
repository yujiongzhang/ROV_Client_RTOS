#ifndef QMOTORLINEEDIT_H
#define QMOTORLINEEDIT_H

#include <QWidget>
#include <QLineEdit>

class qMotorLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit qMotorLineEdit(QWidget *parent = nullptr);

    void updatemotor(float motor_speed);

signals:

};

#endif // QMOTORLINEEDIT_H
