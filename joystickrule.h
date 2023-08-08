#ifndef JOYSTICKRULE_H
#define JOYSTICKRULE_H

#include <QMainWindow>

namespace Ui {
class joyStickRule;
}

class joyStickRule : public QMainWindow
{
    Q_OBJECT

public:
    explicit joyStickRule(QWidget *parent = nullptr);
    ~joyStickRule();

private:
    Ui::joyStickRule *ui;
};

#endif // JOYSTICKRULE_H
