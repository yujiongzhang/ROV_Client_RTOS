#include "navigation.h"
#include "ui_navigation.h"

Navigation::Navigation(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Navigation)
{
    ui->setupUi(this);
}

Navigation::~Navigation()
{
    delete ui;
}
