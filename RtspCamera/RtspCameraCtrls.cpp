// author: zyj
// File: RtspCameraCtrls.cpp

#include "RtspCameraCtrls.h"
#include "ui_RtspCameraCtrls.h"
#include "ffmpeg.h"

RtspCameraCtrls::RtspCameraCtrls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RtspCameraCtrls)
{
    ui->setupUi(this);
    ui->video_record_pushButton->setCheckable(true);
    ui->open_camera_pushButton->setCheckable(true);

    m_ffmpegWidget = (FFmpegWidget*)parent;

    connect(ui->take_one_photo_pushButton,&QPushButton::clicked,this,&RtspCameraCtrls::takeOnePhoto);
    connect(ui->video_record_pushButton,&QPushButton::toggled,this,&RtspCameraCtrls::videoRecord);
    connect(ui->open_camera_pushButton,&QPushButton::toggled,this,&RtspCameraCtrls::openCamera);
}

RtspCameraCtrls::~RtspCameraCtrls()
{
    delete ui;
}

void RtspCameraCtrls::takeOnePhoto()
{
    m_ffmpegWidget->takeOnePhoto();
}

void RtspCameraCtrls::videoRecord(bool checked)
{
    if(!checked){
        m_ffmpegWidget->videoRecodeOFF();
    }
    else{
        m_ffmpegWidget->videoRecodeOn();
    }
}

void RtspCameraCtrls::openCamera(bool checked)
{
    if(!checked){
        m_ffmpegWidget->close();
        return;
    }
    if(isCameraPingable(m_ffmpegWidget->m_cameraIP)){
        m_ffmpegWidget->open();
    }
    else {
        QMessageBox::information(this,"摄像探头未连接","无法ping通\n"+m_ffmpegWidget->m_cameraIP,"   ok   ");
    }

}
