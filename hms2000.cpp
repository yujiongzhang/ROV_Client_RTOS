#include "hms2000.h"
#include <QHostAddress>


Hms2000::Hms2000(QWidget *parent)
    : QWidget{parent}
{
    my_hms2000_thread = new hms2000_thread;
    my_hms2000_thread->set("192.168.0.7",23);

    connect(my_hms2000_thread, &hms2000_thread::receiveImage, this, &Hms2000::updateImage);

    my_hms2000_thread->play();
    my_hms2000_thread->start();
}



void Hms2000::start_sonar()
{
    qDebug()<<"start sonar";
    my_hms2000_thread->start_sonar();
}

void Hms2000::stop_sonar()
{
    qDebug()<<"stop sonar";
    my_hms2000_thread->stop_sonar();
}

void Hms2000::set_default_picture(QImage _image)
{
    updateImage(_image);
}

void Hms2000::take_one_photo()
{
    this->image.save(QString("./sonar/sonar%1.png").arg(QTime::currentTime().toString("HH_mm_ss")),"PNG",-1);
}

void Hms2000::take_one_photo_origin()
{
    this->my_hms2000_thread->take_photo_origin();
}

void Hms2000::set_sonar_config(Sonar_set msg)
{
    qDebug()<< "set sonar config";
    my_hms2000_thread->set_sonar_config(msg);
}

void Hms2000::start_recode()
{
    my_hms2000_thread->startRecode();
}

void Hms2000::stop_recode()
{
    my_hms2000_thread->stopRecode();
}

void Hms2000::paintEvent(QPaintEvent *)
{
//旧
//    QPainter painter(this);
//    painter.drawImage(this->rect(), image);

//新
    int pingNum = my_hms2000_thread->m_nAngle;
    double pingAngle = pingNum / 588 * 360;
    QPainter painter(&image);
    painter.setPen(Qt::red);
    int centerX = image.width()/2;
    int centerY = image.height()/2;
    painter.drawLine(QPoint(centerX,centerY),QPoint(centerX + sin(pingAngle), centerY - cos(pingAngle)));//画上当前扫描的线
    painter.drawImage(this->rect(), image);
}

void Hms2000::updateImage(const QImage receive_image)
{
    this->image = receive_image;
    this->update();
}

