#ifndef ZHIFAN_H
#define ZHIFAN_H

#include <QMainWindow>

namespace Ui {
class zhifan;
}

class zhifan : public QMainWindow
{
    Q_OBJECT

public:
    explicit zhifan(QWidget *parent = nullptr);
    ~zhifan();

private:
    Ui::zhifan *ui;
};

#endif // ZHIFAN_H
