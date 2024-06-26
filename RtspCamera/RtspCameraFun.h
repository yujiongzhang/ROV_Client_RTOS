#ifndef RTSPCAMERAFUN_H
#define RTSPCAMERAFUN_H

#include <iostream>
#include <QString>
#include <QProcess>

// 建议用这个
bool isCameraPingable(const QString& IP);

class RtspCameraFun
{
public:
    RtspCameraFun();
};

#endif // RTSPCAMERAFUN_H
