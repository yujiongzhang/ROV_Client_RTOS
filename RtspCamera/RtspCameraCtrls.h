#ifndef RTSPCAMERACTRLS_H
#define RTSPCAMERACTRLS_H

#include <QWidget>
#include "RtspCameraFun.h"

class FFmpegWidget;

namespace Ui {
class RtspCameraCtrls;
}

class RtspCameraCtrls : public QWidget
{
    Q_OBJECT

public:
    explicit RtspCameraCtrls(QWidget *parent = nullptr);
    ~RtspCameraCtrls();

    FFmpegWidget* m_ffmpegWidget;

    void takeOnePhoto();
    void videoRecord(bool checked);
    void openCamera(bool checked);

private:
    Ui::RtspCameraCtrls *ui;
};

#endif // RTSPCAMERACTRLS_H
