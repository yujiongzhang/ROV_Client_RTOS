#ifndef HMS2000_H
#define HMS2000_H

#include <QObject>
#include <QtWidgets>
#include <QImage>
#include <QTimer>

#include <QThread>

#include"hms2000_thread.h"
#include"msg.h"
#include <QDebug>


class Hms2000 : public QWidget
{
    Q_OBJECT

public:
    explicit Hms2000(QWidget *parent = nullptr);

    void start_sonar();
    void stop_sonar();

    void set_default_picture(QImage _image);

    void take_one_photo();
    void take_one_photo_origin();
    void set_sonar_config(Sonar_set msg);
    void start_recode();
    void stop_recode();

    hms2000_thread *my_hms2000_thread;

protected:
    void paintEvent(QPaintEvent *);


private slots:
    void updateImage(const QImage receive_image);

private:
    //测试定时刷新
    QTimer *timer;

    //内存中的图
    QImage image;

    int m_iValue;
};


#endif // HMS2000_H
