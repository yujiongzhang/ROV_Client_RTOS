
#ifndef SONAR_H
#define SONAR_H

#include <QWidget>

#include <QLabel>
#include <QMainWindow>
#include <QUdpSocket>
#include "definition.h"
#include <QtNetwork/QUdpSocket>
#include <QtMath>
#include <QFile>


class sonar : public QWidget
{
    Q_OBJECT
public:
    explicit sonar(QWidget *parent = nullptr);

    QFile hmsfile;    
    QUdpSocket *m_udp;
    QLabel* m_status;
    QByteArray sendbytearray;

protected:
    void paintEvent(QPaintEvent *);

signals:

};

#endif // SONAR_H
