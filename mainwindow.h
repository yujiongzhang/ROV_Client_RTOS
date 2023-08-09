#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGamepad>
#include <QGamepadManager>
#include <QMetaType>
#include "sendfile.h"
#include "startplot.h"
#include "pidsets.h"
#include "recodefile.h"
#include "joystickrule.h"
#include <QVector>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


signals:
    void startConnect(unsigned short,QString);
    void startConnect2(unsigned short,QString);
    void sendFile(QString path);
    void sendMsg(QString msg);
    void rev_key2(double t,double mun);
    void rev_key1(double t,double mun);
    void s_brightness(int value);
    void s_servo_angle(int8_t value);
    void s_target_depth(float value);

private slots:
    void on_connectServer_clicked();

    void on_actionstartPlot_triggered();

    void on_actionhelp_triggered();

    void virtualtimeDataSlot();

    void on_actionPIDsets_triggered();

    void on_connectServer_2_clicked();

    void on_lightON_clicked();

    void on_lightOFF_clicked();

    void on_brightness_sliderReleased();

    void on_steering_gear_angle_sliderReleased();

    void on_cameraON_clicked();

    void on_cameraOFF_clicked();

    void on_actionrecodefile_triggered();

    void on_videoRecodeON_clicked();

    void on_videoRecodeOFF_clicked();

    void on_pictureRecode_clicked();

    void on_HDvideo_clicked();

    void on_actionJoyStick_rule_triggered();

    
    void on_action_radioButton_clicked();

    void on_common_radioButton_clicked();

    void on_camera_detection_ON_clicked();

    void on_camera_detection_OFF_clicked();

    void on_camera_detection_recode_clicked();

    void on_servo_angle_up_clicked();

    void on_servo_angle_down_clicked();

    void on_sonar_ON_clicked();

    void on_sonar_OFF_clicked();

    void on_sonar_is_main_button_clicked();

    void on_camera_is_main_button_clicked();

    void on_camera2_is_main_button_clicked();

    void on_set_depth_target_clicked();


    void on_sonar_Recode_clicked();

private:
    Ui::MainWindow *ui;
    startPlot * my_plot;
    QTimer virtual_dataTimer;
    PIDSets * my_pidsets;
    recodeFile * my_recodefile;

    QString camera1_urls;
    QString camera2_urls;

    sendfile * worker;//发送消息
    sendfile * worker2;//接收消息

    QGamepad * m_gamepad;//手柄对象

    int connect_state_1;//发送消息的端口是否连接 1为连接，0为不连接
    int connect_state_2;//接收消息的端口是否连接 1为连接，0为不连接

    int light_on;
    int camera_status;//0 表示 主视相机在主显示区 || 1 表示 检测相机在主显示区 || 2 表示 声纳在主显示区
    int rovstate_pre; //记录上一次收到的ROV状态
    int depth_hold_on;//0 表示 未开定深 || 1 表示 开启定深
    int alltitude_hold_on; //0 表示 未开定艏 || 1 表示 开启定艏

    void uiInit();//初始化ui上的一些值
    void paramInit();//初始化Mainwindow中的参数值

    void open_worker1_connects();
    void close_worker1_connects();


    void process_ROV_status(Robot_status_DATA f_robot_status_data);//处理接收到的机器人状态信息
    void process_ROV_txtmsg(char* msg);//处理接收到的机器人文本信息
    void update_status(uint8_t run_mode);//处理接收到的机器人状态信息--用于更新状态
};
#endif // MAINWINDOW_H
