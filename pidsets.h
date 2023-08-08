#ifndef PIDSETS_H
#define PIDSETS_H

#include <QMainWindow>
#include "msg.h"
#include <QMetaType>


namespace Ui {
class PIDSets;
}

class PIDSets : public QMainWindow
{
    Q_OBJECT

public:
    explicit PIDSets(QWidget *parent = nullptr);
    ~PIDSets();

    void PID_set_init();

    void import_pid_config(QString _file);
    void save_pid_config(QString _file);

signals:
    void s_pidset(PIDs_set_DATA v_pidset);

private slots:
    void on_PID_set_button_clicked();

    void on_PID_zero_set_button_clicked();

    void on_import_config1_pushButton_clicked();

    void on_save_config1_pushButton_clicked();

    void on_import_config2_pushButton_clicked();

    void on_save_config2_pushButton_clicked();

    void on_import_config3_pushButton_clicked();

    void on_save_config3_pushButton_clicked();

    void on_import_config4_pushButton_clicked();

    void on_save_config4_pushButton_clicked();

private:
    Ui::PIDSets *ui;
    struct PIDs_set_DATA pids_set_data; //设置pid参数的DATA中的结构体
};

#endif // PIDSETS_H
