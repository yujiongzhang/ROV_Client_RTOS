// author: zyj
// File: recodefile.cpp

#include "recodefile.h"
#include<QThread>

recodeFile::recodeFile(QObject *parent) : QObject(parent)
{
    qDebug() << "创建数据记录线程 ";
    filename = QString("./recode/recode_%1_%2.txt").arg(QDate::currentDate().toString("yyyy_MM_dd"),QTime::currentTime().toString("hh_mm_ss"));
    ofs.open(filename.toStdString());
    ofs<<"time stamp"<<" | "<<"depth"<<" | "<<"yaw" << " | "<<"roll"<<" | "<<"pitch"<<std::endl;
    ofs.close();
}

recodeFile::~recodeFile()
{
    if(ofs.is_open())
    {
        ofs.close();
        qDebug()<<"file close successful!";
    }
    qDebug() << "关闭数据记录线程 ";
}

void recodeFile::writeRecode(Robot_status_DATA _status)
{
//    qDebug() << "数据记录子线程地址: " << QThread::currentThread();
    ofs.open(filename.toStdString(),std::ios_base::app); //以追加的方式写入文件

    if (ofs.is_open()) {
        ofs<<QTime::currentTime().toString("HH:mm:ss:zzz").toStdString();
        ofs<<' ';
        ofs<<_status.ROV_depth;
        ofs<<' ';
        ofs<<_status.ROV_IMU_data.yaw;
        ofs<<' ';
        ofs<<_status.ROV_IMU_data.roll;
        ofs<<' ';
        ofs<<_status.ROV_IMU_data.pitch;
        ofs<<std::endl;
        ofs.close();
    } else {
        qDebug() << "make file error" << filename;
    }
}

