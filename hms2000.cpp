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
    this->image.save(QString("./sonar%1.png").arg(QTime::currentTime().toString("HH_mm_ss")),"PNG",-1);
}

void Hms2000::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(this->rect(), image);
}

void Hms2000::updateImage(const QImage receive_image)
{
    this->image = receive_image;
    this->update();
}

