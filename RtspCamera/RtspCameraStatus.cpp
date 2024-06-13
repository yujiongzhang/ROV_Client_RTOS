#include "RtspCameraStatus.h"
#include "ui_RtspCameraStatus.h"

RtspCameraStatus::RtspCameraStatus(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RtspCameraStatus)
{
    ui->setupUi(this);
}

RtspCameraStatus::~RtspCameraStatus()
{
    delete ui;
}
