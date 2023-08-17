#include "help.h"
#include "ui_help.h"

help::help(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::help)
{
    this->setAttribute(Qt::WA_DeleteOnClose,true);
    ui->setupUi(this);
}

help::~help()
{
    delete ui;
}
