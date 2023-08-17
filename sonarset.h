#ifndef SONARSET_H
#define SONARSET_H

#include <QMainWindow>
#include <QDebug>
#include "msg.h"

namespace Ui {
class SonarSet;
}

class SonarSet : public QMainWindow
{
    Q_OBJECT


signals:
    void s_sonar_set(Sonar_set msg);

public:
    explicit SonarSet(QWidget *parent = nullptr);
    ~SonarSet();

private slots:
    void on_sonar_default_config_PushButton_clicked();

    void on_sonar_set_PushButton_clicked();

private:
    Ui::SonarSet *ui;
};

#endif // SONARSET_H
