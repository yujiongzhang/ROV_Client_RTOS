// author: zyj
// File: ffmpeg.cpp

#include "ffmpeg.h"
#include<QTime>

FFmpegThread::FFmpegThread(QObject *parent) : QThread(parent)
{
    setObjectName("FFmpegThread");
    stopped = false;
    isPlay = false;
    isRecord = false;
    isRecord_start = false;

    frameFinish = false;
    videoWidth = 0;
    videoHeight = 0;
    oldWidth = 0;
    oldHeight = 0;
    videoStreamIndex = -1;
    audioStreamIndex = -1;

    url = "";

    buffer = NULL;
    avPacket = NULL;
    avFrame = NULL;
    avFrame2 = NULL;
    avFrame3 = NULL;
    avFormatContext = NULL;
    videoCodec = NULL;
    audioCodec = NULL;
    swsContext = NULL;

    codec = NULL; // 解码器
    output_format_ctx = NULL;
    output_url = "1208.mp4";
    video_stream = NULL;

    options = NULL;
    videoDecoder = NULL;
    audioDecoder = NULL;

    //初始化注册FFmpeg
    FFmpegThread::initlib();
}

//一个软件中只需要初始化一次就行
void FFmpegThread::initlib()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    static bool isInit = false;
    if (!isInit) {
        //注册库中所有可用的文件格式和解码器
        av_register_all();

        //初始化网络流格式,使用网络流时必须先执行
        avformat_network_init();

        isInit = true;
        qDebug() << TIMEMS << "init ffmpeg lib ok" << " version:" << FFMPEG_VERSION;

    }
}

bool FFmpegThread::init()
{
    //在打开码流前指定各种参数比如:探测时间/超时时间/最大延时等
    //设置缓存大小,1080p可将值调大
    av_dict_set(&options, "buffer_size", "8192000", 0);
    //以tcp方式打开,如果以udp方式打开将tcp替换为udp
    av_dict_set(&options, "rtsp_transport", "tcp", 0);
    //设置超时断开连接时间,单位微秒,3000000表示3秒
    av_dict_set(&options, "stimeout", "3000000", 0);
    //设置最大时延,单位微秒,1000000表示1秒
    av_dict_set(&options, "max_delay", "1000000", 0);
    //自动开启线程数
    av_dict_set(&options, "threads", "auto", 0);

    //打开视频流
    avFormatContext = avformat_alloc_context();

    int result = avformat_open_input(&avFormatContext, url.toStdString().data(), NULL, &options);
    if (result < 0) {
        qDebug() << TIMEMS << "open input error" << url;
        return false;
    }

    //释放设置参数
    if (options != NULL) {
        av_dict_free(&options);
    }

    //获取流信息
    result = avformat_find_stream_info(avFormatContext, NULL);
    if (result < 0) {
        qDebug() << TIMEMS << "find stream info error";
        return false;
    }

    //----------视频流部分开始----------
    if (1) {
        videoStreamIndex = av_find_best_stream(avFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, &videoDecoder, 0);
        if (videoStreamIndex < 0) {
            qDebug() << TIMEMS << "find video stream index error";
            return false;
        }

        //获取视频流
        AVStream *videoStream = avFormatContext->streams[videoStreamIndex];

        //获取视频流解码器,或者指定解码器
        videoCodec = videoStream->codec;
        videoDecoder = avcodec_find_decoder(videoCodec->codec_id);
        //videoDecoder = avcodec_find_decoder_by_name("h264_qsv");
        if (videoDecoder == NULL) {
            qDebug() << TIMEMS << "video decoder not found";
            return false;
        }

        //设置加速解码
        videoCodec->lowres = videoDecoder->max_lowres;
        videoCodec->flags2 |= AV_CODEC_FLAG2_FAST;

        //打开视频解码器
        result = avcodec_open2(videoCodec, videoDecoder, NULL);
        if (result < 0) {
            qDebug() << TIMEMS << "open video codec error";
            return false;
        }

        //获取分辨率大小
        videoWidth = videoStream->codec->width;
        videoHeight = videoStream->codec->height;

        //如果没有获取到宽高则返回
        if (videoWidth == 0 || videoHeight == 0) {
            qDebug() << TIMEMS << "find width height error";
            return false;
        }

        QString videoInfo = QString("视频流信息 -> 索引: %1  解码: %2  格式: %3  时长: %4 秒  分辨率: %5*%6")
                            .arg(videoStreamIndex).arg(videoDecoder->name).arg(avFormatContext->iformat->name)
                            .arg((avFormatContext->duration) / 1000000).arg(videoWidth).arg(videoHeight);
        qDebug() << TIMEMS << videoInfo;
    }
    //----------视频流部分结束----------

    //预分配好内存
    avPacket = av_packet_alloc();
    avFrame = av_frame_alloc();
    avFrame2 = av_frame_alloc();
    avFrame3 = av_frame_alloc();

    //比较上一次文件的宽度高度,当改变时,需要重新分配内存
    if (oldWidth != videoWidth || oldHeight != videoHeight) {
        int byte = avpicture_get_size(AV_PIX_FMT_RGB32, videoWidth, videoHeight);
        buffer = (uint8_t *)av_malloc(byte * sizeof(uint8_t));
        oldWidth = videoWidth;
        oldHeight = videoHeight;
    }

    //定义像素格式
    AVPixelFormat srcFormat = AV_PIX_FMT_YUV420P;
    AVPixelFormat dstFormat = AV_PIX_FMT_RGB32;
    //通过解码器获取解码格式
    srcFormat = videoCodec->pix_fmt;

    //默认最快速度的解码采用的SWS_FAST_BILINEAR参数,可能会丢失部分图片数据,可以自行更改成其他参数
    int flags = SWS_FAST_BILINEAR;

    //开辟缓存存储一帧数据
    //avpicture_fill((AVPicture *)avFrame3, buffer, dstFormat, videoWidth, videoHeight);
    av_image_fill_arrays(avFrame3->data, avFrame3->linesize, buffer, dstFormat, videoWidth, videoHeight, 1);

    //图像转换
    swsContext = sws_getContext(videoWidth, videoHeight, srcFormat, videoWidth, videoHeight, dstFormat, flags, NULL, NULL, NULL);

    return true;
}

void FFmpegThread::run()
{
    while (!stopped) {
        //根据标志位执行初始化操作
        if (isPlay) {
            this->init();
            isPlay = false;
            continue;
        }

        if (av_read_frame(avFormatContext, avPacket) >= 0) {
            //判断当前包是视频还是音频
            int index = avPacket->stream_index;
            if (index == videoStreamIndex) {

                // 将视频帧数据包发送给视频解码器进行解码。
                frameFinish = avcodec_send_packet(videoCodec, avPacket);
                if (frameFinish < 0) {
                    continue;
                }

                // 接收视频解码器解码后的帧数据。
                // avFrame2 是解码后的视频帧数据。
                frameFinish = avcodec_receive_frame(videoCodec, avFrame2);
                if (frameFinish < 0) {
                    continue;
                }

                if (frameFinish >= 0) {//判断帧数据是否解码成功。
                    //将数据转成一张图片
                    sws_scale(swsContext, (const uint8_t *const *)avFrame2->data, avFrame2->linesize, 0, videoHeight, avFrame3->data, avFrame3->linesize);

                    QImage image((uchar *)buffer, videoWidth, videoHeight, QImage::Format_RGB32);
                    if (!image.isNull()) {
                        emit receiveImage(image);
                    }

                    if(isRecord){
                        if(isRecord_start){ // 刚开始视频录制
                            //函数用于创建输出文件的 AVFormatContext 结构体。

                            QSettings settings;
                            QString videoSavePath = settings.value("savePath/Videos", "./").toString();
                            QString vidoeSaveName = videoSavePath + QString("/video_%1.mp4").arg(QDateTime::currentDateTime().toString("yyyy_MM_dd_HH_mm_ss_zzz"));
                            output_url = vidoeSaveName.toUtf8().constData();
                            qDebug()<<"【录制视频】【开始】文件保存于 " <<  output_url;

                            avformat_alloc_output_context2(&output_format_ctx, nullptr, nullptr, output_url);
                            if (!output_format_ctx) {
                                printf("Could not create output format context");
                            }

                            // 函数用于为输出文件创建新的流。
                            video_stream = avformat_new_stream(output_format_ctx, videoDecoder);
                            if (!video_stream) {
                                printf("Failed to create video stream");
                            }

                            // 将输入文件中的视频流参数复制到输出文件的视频流参数中。
                            if (avcodec_parameters_copy(video_stream->codecpar, avFormatContext->streams[videoStreamIndex]->codecpar) < 0) {
                                printf("Failed to copy codec parameters");
                            }

                            // 将输出文件视频流的编解码器标签（FourCC）设为 0。
                            video_stream->codecpar->codec_tag = 0;

                            if (!(output_format_ctx->oformat->flags & AVFMT_NOFILE)) {
                                if (avio_open(&output_format_ctx->pb, output_url, AVIO_FLAG_WRITE) < 0) {
                                    printf("Could not open output file '%s'", output_url);
                                }
                            }
                            // 写入输出文件的格式头。
                            if (avformat_write_header(output_format_ctx, nullptr) < 0) {
                                printf("Error occurred when opening output file");
                            }

                            start_record_pts = avPacket->pts;
                            start_record_dts = avPacket->dts;
                            isRecord_start = false;
                        }
//                        qDebug()<<"avPacket->pts" << avPacket->pts;
//                        qDebug()<<"avPacket->dts" << avPacket->dts;
                        avPacket->pts = avPacket->pts - start_record_pts + 3600;
                        avPacket->dts = avPacket->dts - start_record_pts + 3600;
//                        qDebug()<<"after avPacket->pts" << avPacket->pts;
//                        qDebug()<<"after avPacket->dts" << avPacket->dts;
                        avPacket->stream_index = video_stream->index;// 将当前数据包的流索引改为输出文件视频流的索引。
                        av_interleaved_write_frame(output_format_ctx, avPacket);//将改变流索引后的数据包写入输出文件。
                    }



                    msleep(1);

                }
            } else if (index == audioStreamIndex) {

            }
        }

        av_packet_unref(avPacket);
        av_freep(avPacket);
        msleep(1);
    }

    if(isRecord){//如果还在录制
        av_write_trailer(output_format_ctx);//写入输出文件的格式尾部。
        avio_closep(&output_format_ctx->pb);// 关闭输出文件的 I/O 上下文。
    }

    //线程结束后释放资源
    free();
    stopped = false;
    isPlay = false;
    isRecord = false;
    qDebug() << TIMEMS << "stop ffmpeg thread";
}

void FFmpegThread::setUrl(const QString &url)
{
    this->url = url;
}

void FFmpegThread::free()
{
    if (swsContext != NULL) {
        sws_freeContext(swsContext);
        swsContext = NULL;
    }

    if (avPacket != NULL) {
        av_packet_unref(avPacket);
        avPacket = NULL;
    }

    if (avFrame != NULL) {
        av_frame_free(&avFrame);
        avFrame = NULL;
    }

    if (avFrame2 != NULL) {
        av_frame_free(&avFrame2);
        avFrame2 = NULL;
    }

    if (avFrame3 != NULL) {
        av_frame_free(&avFrame3);
        avFrame3 = NULL;
    }

    if (videoCodec != NULL) {
        avcodec_close(videoCodec);
        videoCodec = NULL;
    }

    if (audioCodec != NULL) {
        avcodec_close(audioCodec);
        audioCodec = NULL;
    }

    if (avFormatContext != NULL) {
        avformat_close_input(&avFormatContext);
        avFormatContext = NULL;
    }

    if (output_format_ctx != NULL) {
        avformat_free_context(output_format_ctx);// 释放输出文件的格式上下文资源。
        output_format_ctx = NULL;
    }

    av_dict_free(&options);
    //qDebug() << TIMEMS << "close ffmpeg ok";
}

void FFmpegThread::play()
{
    //通过标志位让线程执行初始化
    isPlay = true;
}

void FFmpegThread::pause()
{

}

void FFmpegThread::next()
{

}

void FFmpegThread::stop()
{
    //通过标志位让线程停止
    stopped = true;
}

void FFmpegThread::record()
{
    isRecord = true;
    isRecord_start = true;
}

void FFmpegThread::stopRecord()
{
    isRecord = false;
    av_write_trailer(output_format_ctx);//写入输出文件的格式尾部。
    avio_closep(&output_format_ctx->pb);// 关闭输出文件的 I/O 上下文。
    qDebug()<<"【录制视频】【结束】文件保存于 " <<  output_url;
}

//实时视频显示窗体类
FFmpegWidget::FFmpegWidget(QWidget *parent) : QWidget(parent)
  , m_rtspCameraCtrls(this)
  , m_rtspCameraStatus(this)
{
    thread = new FFmpegThread(this);
    recodeThread = new FFmpegRecodeThread(this);
    connect(thread, SIGNAL(receiveImage(QImage)), this, SLOT(updateImage(QImage)));
    image = QImage();
    isPhotos = false;

    thisIsMain = false;
    thisIsMainChanged = true;


    QImage originalImage(":/RtspCameraResource/RtspCamera_background.png");
    // 创建一个临时QImage，并在其上绘制原始图像
    QImage antialiasedImage(originalImage.size(), QImage::Format_ARGB32);
    antialiasedImage.fill(Qt::transparent);
    QPainter painter(&antialiasedImage);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.drawImage(0, 0, originalImage);// antialiasedImage 现在包含了抗锯齿效果的图像
    this->set_default_picture(antialiasedImage);

//    this->set_default_picture(QImage(":/RtspCameraResource/RtspCamera_background.png"));


}

FFmpegWidget::~FFmpegWidget()
{
    close();
}

void FFmpegWidget::set_default_picture(QImage _image)
{
    updateImage(_image);
}

void FFmpegWidget::setCameraIP(const QString &ip)
{
    this->m_cameraIP = ip;
}

void FFmpegWidget::ChangeSizeDisplay(bool _sizeDisplayMode)
{
    thisIsMainChanged = true;
    thisIsMain = _sizeDisplayMode;
    LayoutCtrls();
    thisIsMainChanged = false;
}

void FFmpegWidget::LayoutCtrls()
{
    QRect r = rect();
    m_rtspCameraCtrls.setGeometry(r.width()-m_rtspCameraCtrls.width()-r.width()*0.05,r.height()-m_rtspCameraCtrls.height(),m_rtspCameraCtrls.width(),m_rtspCameraCtrls.height());
    m_rtspCameraStatus.setGeometry(r.width()-m_rtspCameraStatus.width(),r.height()-m_rtspCameraStatus.height(),m_rtspCameraStatus.width(),m_rtspCameraStatus.height());

    if(! thisIsMain){
        m_rtspCameraCtrls.setVisible(false);
        if(thread->isRecord){
            m_rtspCameraStatus.setVisible(true);
        }
        else{
            m_rtspCameraStatus.setVisible(false);
        }
    }
    else{
        m_rtspCameraCtrls.setVisible(true);
        m_rtspCameraStatus.setVisible(false);
    }
}

void FFmpegWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    LayoutCtrls();
}


void FFmpegWidget::paintEvent(QPaintEvent *)
{
    if (image.isNull()) {
        return;
    }

    //qDebug() << TIMEMS << "paintEvent" << objectName();
    QPainter painter(this);

    painter.drawImage(this->rect(), image);
}

void FFmpegWidget::updateImage(const QImage &image)
{
    //this->image = image.copy();
    this->image = image;
    this->update();
}

void FFmpegWidget::setUrl(const QString &url)
{
    thread->setUrl(url);
}

void FFmpegWidget::open()
{
    //qDebug() << TIMEMS << "open video" << objectName();

    clear();
    thread->play();
    thread->start();

}

void FFmpegWidget::pause()
{
    thread->pause();
}

void FFmpegWidget::next()
{
    thread->next();
}

void FFmpegWidget::close()
{
    //qDebug() << TIMEMS << "close video" << objectName();
    if (thread->isRunning()) {
        thread->stop();
        thread->quit();
        thread->wait(500);
    }

    QTimer::singleShot(1, this, SLOT(clear()));
}

void FFmpegWidget::clear()
{
    image = QImage();
    update();
}

void FFmpegWidget::videoRecodeOn()
{

   thread->record();
}

void FFmpegWidget::videoRecodeOFF()
{
    thread->stopRecord();
}


void FFmpegWidget::photos(const QImage &image)
{
    if(isPhotos)
    {
        QSettings settings;
        QString pictureSavePath = settings.value("savePath/Pictures").toString();
        QString pictureSaveName = pictureSavePath + QString("/picture_%1.png").arg(QDateTime::currentDateTime().toString("yyyy_MM_dd_HH_mm_ss_zzz"));

        image.save(pictureSaveName,"PNG",-1);
        isPhotos = false;
        qDebug()<<"【拍照】      文件保存于 " <<  pictureSavePath;
    }
    disconnect(thread, &FFmpegThread::receiveImage, this, &FFmpegWidget::photos);
}


void FFmpegWidget::takeOnePhoto()
{
    isPhotos = true;
    connect(thread, &FFmpegThread::receiveImage, this, &FFmpegWidget::photos);
}

FFmpegRecodeThread::FFmpegRecodeThread(QObject *parent) : QThread(parent)
{
    this->isRecode = false;

}

void FFmpegRecodeThread::run()
{
    while(isRecode){
        this->image.save(QString("./video/picture%1.png").arg(QTime::currentTime().toString("HH_mm_ss_zzz")),"PNG",-1);
    }
}

void FFmpegRecodeThread::receiveImage(const QImage &_image)
{
    this->image = _image;
}

void FFmpegRecodeThread::play()
{
    this->isRecode = true;
}

void FFmpegRecodeThread::stop()
{
    this->isRecode = false;
}


