#include "zhifan.h"
#include "ui_zhifan.h"

zhifan::zhifan(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::zhifan)
{
    ui->setupUi(this);
}

zhifan::~zhifan()
{
    delete ui;
}
