#ifndef HMS2000_THREAD_H
#define HMS2000_THREAD_H

#include <QObject>
#include <QImage>
#include <QTimer>

#include <QThread>
#include <QDebug>
#include <QtMath>
#include <QFile>
#include <QUdpSocket>
#include "definition.h"


class hms2000_thread : public QThread
{
    Q_OBJECT
public:
    explicit hms2000_thread(QObject *parent = nullptr);
    void param_init();

    void update_sonar_image();
    void calDelta();
    void InitColortable();//从bin文件中读取设置好的颜色表
    void DrawColortable(QRgb *bits);//绘制颜色表

    void start_receive_data();

    void process_receive_data();

    //HMS2000图像绘制  角度angle测得的一组数据buf 一组数据长度len=300
    //步距为4，需要绘制的角度宽度为step=1.224° 实测数据每2.448°得到一组 由于前后两组平均后最后的步距角为1.224°
    //步距为1，需要绘制的角度宽度为step=0.306° 实测数据每0.612°得到一组 由于前后两组平均后最后的步距角为0.306°
    void Scan(double angle, uchar *buf, uint len, double step);

    void start_sonar();
    void stop_sonar();

protected:
    void run();

private:
    volatile bool isPlay;           //播放视频标志位
    volatile bool stopped;
    int deviceID;

    QUdpSocket *m_udp;
    QByteArray sendbytearray;

    QImage* image1;
    QRgb *m_pBits;//指向图像的像素点

    QTimer * cmdTimer;


    QString connectAdd;//声纳地址
    quint16 connectPort;//声纳端口

    QFile hmsfile;

    uint m_nScanLineMax = 0;
    uint m_nScanPerAngle = 0;
    uint m_dwDeltaX[4800];
    uint m_dwDeltaY[4800];
    uint m_mycolorTable[256];	//颜色表

    uint m_nR = 300; //画图的半径
    uint m_width;
    uint m_height;
    uint m_radius;
    uint m_nOrgY;
    uint m_nOrgX;

    uchar* hms_bufnow;		//数据
    uchar hms_buf1[HMSPPIDATASIZE];
    uchar hms_buf2[HMSPPIDATASIZE];
    uchar hms_midBuf[HMSPPIDATASIZE];
    int m_nAngle; //1~588 一次+1  n表示int, 声纳上传的angle，不为实际角度
    int m_nLastAngle;  //初始化为-1，记录上次glb_nAngle
    int m_nMaxAngle;//初始化为588
    double m_angle;
    double m_scan_step;



signals:
    void receiveImage(QImage);

public slots:
    //播放视频对象
    void set(QString sonar_add, quint16 sonar_port);
    void play();
    void stop();
};


#endif // HMS2000_THREAD_H
