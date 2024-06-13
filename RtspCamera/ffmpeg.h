#ifndef FFMPEG_H
#define FFMPEG_H

#include <QtGui>
#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
#include <QtWidgets>
#endif

#include "ffmpeghead.h"
#include "ispingable.h"

#include "RtspCameraCtrls.h"
#include "RtspCameraStatus.h"

class FFmpegThread : public QThread
{
    Q_OBJECT
public:
    explicit FFmpegThread(QObject *parent = 0);
    static void initlib();

    QString url;                    //视频流地址

    volatile bool stopped;          //线程停止标志位
    volatile bool isPlay;           //播放视频标志位
    volatile bool isRecord;           //录制视频标志位
    volatile bool isRecord_start;           //录制视频开始标志位

protected:
    void run();

private:


    int frameFinish;                //一帧完成
    int videoWidth;                 //视频宽度
    int videoHeight;                //视频高度
    int oldWidth;                   //上一次视频宽度
    int oldHeight;                  //上一次视频高度
    int videoStreamIndex;           //视频流索引
    int audioStreamIndex;           //音频流索引

    int64_t start_record_pts;       //录像开始显示时间戳
    int64_t start_record_dts;       //录像开始解码时间戳



    uint8_t *buffer;                //存储解码后图片buffer
    AVPacket *avPacket;             //包对象
    AVFrame *avFrame;               //帧对象
    AVFrame *avFrame2;              //帧对象
    AVFrame *avFrame3;              //帧对象
    AVFormatContext *avFormatContext;//格式对象
    AVCodecContext *videoCodec;     //视频解码器
    AVCodecContext *audioCodec;     //音频解码器
    SwsContext *swsContext;         //处理图片数据对象

    AVCodec* codec; // 解码器
    AVFormatContext* output_format_ctx;//输出格式对象
    const char* output_url;
    AVStream* video_stream; //输出视频流

    AVDictionary *options;          //参数对象

    AVCodec *videoDecoder;          //视频解码
    AVCodec *audioDecoder;          //音频解码

signals:
    //收到图片信号
    void receiveImage(const QImage &image);

public slots:
    //设置视频流地址
    void setUrl(const QString &url);

    //初始化视频对象
    bool init();
    //释放对象
    void free();
    //播放视频对象
    void play();
    //暂停播放
    void pause();
    //继续播放
    void next();
    //停止采集线程
    void stop();
    //开始录制
    void record();
    //停止录制
    void stopRecord();
};


class FFmpegRecodeThread : public QThread
{
    Q_OBJECT
public:
    explicit FFmpegRecodeThread(QObject *parent = 0);
    static void initlib();

protected:
    void run();

public:

    volatile bool isRecode;           //播放视频标志位
    //接收图像并保存
    void videoRecode(const QImage &image);

    void receiveImage(const QImage &_image);

    void play();
    void stop();


signals:

private:
    QImage image;

};



//实时视频显示窗体类
class FFmpegWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FFmpegWidget(QWidget *parent = 0);
    ~FFmpegWidget();

    void set_default_picture(QImage _image);


    RtspCameraCtrls m_rtspCameraCtrls;
    RtspCameraStatus m_rtspCameraStatus;

    QString m_cameraIP; //用来测试相机是否能ping通，防止相机无法打开造成死机
    void setCameraIP(const QString &ip);


    bool thisIsMain;
    bool thisIsMainChanged;
    void ChangeSizeDisplay(bool _sizeDisplayMode);

    void LayoutCtrls();

    // Overrides
    void resizeEvent(QResizeEvent* event)  Q_DECL_OVERRIDE;
protected:   
    void paintEvent(QPaintEvent *);

private:
    FFmpegThread *thread;
    FFmpegRecodeThread *recodeThread;
    QImage image;

    volatile bool isPhotos;           //播放视频标志位


private slots:
    //接收图像并绘制
    void updateImage(const QImage &image);

    //接收单张图片
    void photos(const QImage &image);



public slots:
    //设置视频流地址
    void setUrl(const QString &url);

    //打开设备
    void open();
    //暂停
    void pause();
    //继续
    void next();
    //关闭设备
    void close();
    //清空
    void clear();

    //开始录像
    void videoRecodeOn();
    //停止录像
    void videoRecodeOFF();

    //拍照
    void takeOnePhoto();



};

#endif // FFMPEG_H
