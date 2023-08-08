#include "recodefile.h"
#include<QThread>

recodeFile::recodeFile(QObject *parent) : QObject(parent)
{
    qDebug() << "创建数据记录线程 ";
    fp = fopen("recode.txt","w");
    if(fp == NULL){
        qDebug()<<"file error";
    }
}

recodeFile::~recodeFile()
{
    fclose(fp);
    qDebug()<<"file close successful!";
    qDebug() << "关闭数据记录线程 ";
}

void recodeFile::writeRecode(Robot_status_DATA _status)
{
    qDebug() << "数据记录子线程地址: " << QThread::currentThread();

    QString imu_euler_msg_to_str;
    imu_euler_msg_to_str.append(QTime::currentTime().toString("HH:mm:ss:zzz"));
    imu_euler_msg_to_str.append("  ");
    imu_euler_msg_to_str.append( QString::number(_status.ROV_depth,'f'));
    imu_euler_msg_to_str.append("  ");
    imu_euler_msg_to_str.append( QString::number(_status.ROV_IMU_data.pitch,'f'));
    imu_euler_msg_to_str.append("  ");
    imu_euler_msg_to_str.append( QString::number(_status.ROV_IMU_data.roll,'f'));
    imu_euler_msg_to_str.append("  ");
    imu_euler_msg_to_str.append( QString::number(_status.ROV_IMU_data.yaw,'f'));
    imu_euler_msg_to_str.append("  ");
    imu_euler_msg_to_str.append( QString::number(_status.ROV_IMU_data.ax,'f'));
    imu_euler_msg_to_str.append("  ");
    imu_euler_msg_to_str.append( QString::number(_status.ROV_IMU_data.ay,'f'));
    imu_euler_msg_to_str.append("  ");
    imu_euler_msg_to_str.append( QString::number(_status.ROV_IMU_data.az,'f'));
    imu_euler_msg_to_str.append("  ");
    imu_euler_msg_to_str.append( QString::number(_status.ROV_IMU_data.gx,'f'));
    imu_euler_msg_to_str.append("  ");
    imu_euler_msg_to_str.append( QString::number(_status.ROV_IMU_data.gy,'f'));
    imu_euler_msg_to_str.append("  ");
    imu_euler_msg_to_str.append( QString::number(_status.ROV_IMU_data.gz,'f'));
    imu_euler_msg_to_str.append("\n");

    QByteArray imu_msg_to_chr = imu_euler_msg_to_str.toLatin1();

    fputs(imu_msg_to_chr.data(),fp);

}

