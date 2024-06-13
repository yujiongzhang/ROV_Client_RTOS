#ifndef RTSPCAMERASTATUS_H
#define RTSPCAMERASTATUS_H

#include <QWidget>

namespace Ui {
class RtspCameraStatus;
}

class RtspCameraStatus : public QWidget
{
    Q_OBJECT

public:
    explicit RtspCameraStatus(QWidget *parent = nullptr);
    ~RtspCameraStatus();

private:
    Ui::RtspCameraStatus *ui;
};

#endif // RTSPCAMERASTATUS_H
