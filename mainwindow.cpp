#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QThread>

#include <QFileDialog>
#include "help.h"
#include "zhifan.h"

#include "aspectratiowidget.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qRegisterMetaType<int8_t>("int8_t");//跨线程的信号和槽的参数传递中,参数的类型是自定义的类型
    ui->setupUi(this);

    qDebug() << "主线程地址: " << QThread::currentThread();

    uiInit();//界面初始化（按钮、图片等）
    paramInit();//参数初始化


    //创建线程对象
    QThread * t1 = new QThread;
    QThread * t2 = new QThread;
    QThread * t3 = new QThread;
    //创建任务对象
    worker = new(sendfile);//发送指令的套接字
    worker2 = new(sendfile);//用于接收信息的套接字

    //连接手柄
    m_gamepad = new QGamepad(0);

    //把对象放到不同线程中
    worker->moveToThread(t1);
    m_gamepad->moveToThread(t2);
    worker2->moveToThread(t3);

    connect(this,&MainWindow::startConnect,worker,&sendfile::connectServer);
    connect(this,&MainWindow::sendFile,worker,&sendfile::sendFile);
    connect(this,&MainWindow::sendMsg,worker,&sendfile::sendMsg);

    connect(this,&MainWindow::startConnect2,worker2,&sendfile::connectServer);

    //处理主线程发送的信号
    connect(worker,&sendfile::connectOK,this,[=](){
        QMessageBox::information(this,"服务器连接","端口1已经成功连接了服务器，恭喜！");
        open_worker1_connects();
        ui->connectServer->setEnabled(false);
    });
    connect(worker,&sendfile::gameover,this,[=](){
//        t1->quit();
//        t1->wait();
//        worker->deleteLater();
//        t1->deleteLater();
        close_worker1_connects();
        QMessageBox::information(this,"服务器断开","端口1已经断开");
        ui->connectServer->setEnabled(true);
    });

    connect(worker2,&sendfile::connectOK,this,[=](){
        QMessageBox::information(this,"服务器连接","端口2已经成功连接了服务器，恭喜！");
        ui->connectServer_2->setEnabled(false);
    });
    connect(worker2,&sendfile::gameover,this,[=](){
//        t3->quit();
//        t3->wait();
//        worker2->deleteLater();
//        t3->deleteLater();
        QMessageBox::information(this,"服务器断开","端口2已经断开");
        ui->status_label->setText("未连接：没有接收到ROV的消息");
        ui->connectServer_2->setEnabled(true);
    });


    connect(m_gamepad,&QGamepad::connectedChanged,this,[=](bool isconnect){
        if(isconnect)
        {
            QMessageBox::information(this,"手柄连接","已经成功连接了手柄，恭喜！");
        }
        else{QMessageBox::information(this,"手柄连接","手柄未连接");}
    });

    t1->start();
    t2->start();
    t3->start();


    // 端口2 接收消息并显示
    connect(worker2,&sendfile::recvMsg,ui->txt_msg,&QTextEdit::append);


    connect(worker2,&sendfile::s_ROV_status,this,&MainWindow::process_ROV_status);// 端口2 接收处理1类消息
    connect(worker2,&sendfile::s_ROV_txtmsg,this,&MainWindow::process_ROV_txtmsg);// 端口2 接收处理2类消息

    // 设置一个重复调用 MainWindow::realtimeDataSlot 的计时器
    connect(&virtual_dataTimer, SIGNAL(timeout()), this, SLOT(virtualtimeDataSlot()));
    virtual_dataTimer.start(0); // Interval 0 means to refresh as fast as possib-》le
//    connect(this,&MainWindow::rev_key2,my_plot,&startPlot::addDataSlot);

}


MainWindow::~MainWindow()
{
    delete ui;
}

//初始化ui的一些值
void MainWindow::uiInit()
{

//    针对simu-rov-server
//    ui->ip->setText("127.0.0.1");
//    ui->port->setText("5501");
//    ui->ip_2->setText("127.0.0.1");
//    ui->port_2->setText("5502");

//    针对ROV下位机
//    ui->ip->setText("192.168.1.25");
//    ui->port->setText("5001");
//    ui->ip_2->setText("192.168.1.25");
//    ui->port_2->setText("5002");

    QFile file(":/config/config/rovClientConfig.json");
    file.open(QFile::ReadOnly);
    QByteArray all = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(all);

    if(doc.isObject())
        {
            QJsonObject obj = doc.object();

            ui->ip->setText(obj.value("ip1").toString());
            ui->port->setText(obj.value("port1").toString());
            ui->ip_2->setText(obj.value("ip2").toString());
            ui->port_2->setText(obj.value("port2").toString());

            camera1_urls.append(obj.value("camera1_urls").toString());
            camera2_urls.append(obj.value("camera2_urls").toString());
            qDebug() << "camera1 RTSP URL is:" << camera1_urls;
            qDebug() << "camera2 RTSP URL is:" << camera2_urls;
        }

    ui->lightOFF->setEnabled(false);
    ui->cameraOFF->setEnabled(false);
    ui->sonar_OFF->setEnabled(false);
    ui->camera_detection_OFF->setEnabled(false);
    ui->camera_is_main_button->setEnabled(false);
    ui->status_label->setText("未连接：没有接收到ROV的消息");
    ui->steering_gear_angle_label->setText(QString::number(ui->steering_gear_angle->value()));
    ui->brightness_label->setText("0");

    //---模型形态---------
    ui->openGLWidget->set_viewat(QVector3D(0.0f, -1.3f, 0.3f), QVector3D(0.0f,0.0f,0.0f), QVector3D(0.0f,0.0f,1.0f));
    ui->openGLWidget_yaw->set_viewat(QVector3D(0.0f, 0.0f, 1.5f), QVector3D(0.0f,0.0f,0.0f), QVector3D(0.0f,1.0f,0.0f));

    //---相机、声纳显示比例-------
    ui->aspect_ratio_weight_1->set_ration(16,9);
    ui->aspect_ratio_weight_1->set_layout(ui->camera);

    ui->aspect_ratio_weight_2->set_ration(16,9);
    ui->aspect_ratio_weight_2->set_layout(ui->camera_2);

    ui->aspect_ratio_weight_3->set_ration(8,7);
    ui->aspect_ratio_weight_3->set_layout(ui->sonar);

    ui->aspect_ratio_weight_0->set_ration(4,4);
    ui->aspect_ratio_weight_0->set_layout(ui->openGLWidget);

    ui->aspect_ratio_weight_00->set_ration(4,4);
    ui->aspect_ratio_weight_00->set_layout(ui->openGLWidget_yaw);

    ui->camera->set_default_picture(QImage(":/images/images/camera1.png"));
    ui->camera_2->set_default_picture(QImage(":/images/images/camera2.png"));
    ui->sonar->set_default_picture(QImage(":/images/images/sonar.png"));
}

void MainWindow::paramInit()
{
    light_on = 0;
    camera_status = 0;
    speed_adjust = SPEED_ADJUST_COMMON;
}

void MainWindow::on_connectServer_clicked()
{
    QString ip = ui->ip->text();
    unsigned short port = ui->port->text().toUShort();
    emit startConnect(port,ip);
}

void MainWindow::on_connectServer_2_clicked()
{
    QString ip2 = ui->ip_2->text();
    unsigned short port2 = ui->port_2->text().toUShort();
    emit startConnect2(port2,ip2);
}



void MainWindow::on_actionstartPlot_triggered()
{
    my_plot = new(startPlot);//打开绘图窗口
    my_plot->show();

    // 当 my_plot 窗口关闭后，相关的connect也会消失，此处无需disconnect
    connect(this->worker2,&sendfile::s_ROV_status,my_plot,&startPlot::updateSlot);// 画图窗口处理 接收到的1类消息

}

void MainWindow::on_actionhelp_triggered()
{
    help * my_help = new(help);
    my_help->show();
}

void MainWindow::on_actionJoyStick_rule_triggered()
{
    joyStickRule * my_joyStickRule = new(joyStickRule);
    my_joyStickRule->show();
}

void MainWindow::on_actionPIDsets_triggered()
{
    my_pidsets = new(PIDSets);
    my_pidsets->show();
    connect(my_pidsets, &PIDSets::s_pidset,this->worker,&sendfile::sendMsgPID);
}


void MainWindow::on_actionrecodefile_triggered()
{
    if(ui->actionrecodefile->text() == "recodefile")
    {
        QThread *t_recode = new(QThread);
        my_recodefile = new(recodeFile);
        my_recodefile->moveToThread(t_recode);
        t_recode->start();
        connect(this->worker2,&sendfile::s_ROV_status,my_recodefile,&recodeFile::writeRecode);// 画图窗口处理 接收到的1类消息

        ui->actionrecodefile->setText("stop recode");
    }
    else if(ui->actionrecodefile->text() == "stop recode")
    {
        my_recodefile->deleteLater();
        ui->actionrecodefile->setText("recodefile");
    }

}

void MainWindow::virtualtimeDataSlot()//通过时间来创建一个虚拟的模拟下位机上传
{
    static QTime timeStart2 = QTime::currentTime();
    double key2 = timeStart2.msecsTo(QTime::currentTime())/1000.0; // 项目开始后经过的时间,单位 s
    static double lastPointKey2 = 0;
    if (key2-lastPointKey2 > 0.01) // 最多每10ms添加一点
    {
        emit rev_key1(key2, qSin(key2));
        emit rev_key2(key2, qSin(key2)+qSin(10*key2)+qSin(100*key2));
        lastPointKey2 = key2;
    }
}


// 处理ROV上传的状态信息
void MainWindow::process_ROV_status(Robot_status_DATA f_robot_status_data)
{
//    ui->thrSpeed1->setText(QString::number(f_robot_status_data.ROV_motors_status.thruster_speed_1));
//    ui->thrSpeed2->setText(QString::number(f_robot_status_data.ROV_motors_status.thruster_speed_2));
//    ui->thrSpeed3->setText(QString::number(f_robot_status_data.ROV_motors_status.thruster_speed_3));
//    ui->thrSpeed4->setText(QString::number(f_robot_status_data.ROV_motors_status.thruster_speed_4));
//    ui->thrSpeed5->setText(QString::number(f_robot_status_data.ROV_motors_status.thruster_speed_5));
//    ui->thrSpeed6->setText(QString::number(f_robot_status_data.ROV_motors_status.thruster_speed_6));
//    ui->craSpeed1->setText(QString::number(f_robot_status_data.ROV_motors_status.crawler_motor_1));
//    ui->craSpeed2->setText(QString::number(f_robot_status_data.ROV_motors_status.crawler_motor_2));

    ui->thrSpeed1->updatemotor(f_robot_status_data.ROV_motors_status.thruster_speed_1);
    ui->thrSpeed2->updatemotor(f_robot_status_data.ROV_motors_status.thruster_speed_2);
    ui->thrSpeed3->updatemotor(f_robot_status_data.ROV_motors_status.thruster_speed_3);
    ui->thrSpeed4->updatemotor(f_robot_status_data.ROV_motors_status.thruster_speed_4);
    ui->thrSpeed5->updatemotor(f_robot_status_data.ROV_motors_status.thruster_speed_5);
    ui->thrSpeed6->updatemotor(f_robot_status_data.ROV_motors_status.thruster_speed_6);
    ui->craSpeed1->updatemotor(f_robot_status_data.ROV_motors_status.crawler_motor_1);
    ui->craSpeed2->updatemotor(f_robot_status_data.ROV_motors_status.crawler_motor_2);

    ui->yaw->setText(QString::number(f_robot_status_data.ROV_IMU_data.yaw));
    ui->pit->setText(QString::number(f_robot_status_data.ROV_IMU_data.pitch));
    ui->rol->setText(QString::number(f_robot_status_data.ROV_IMU_data.roll));

    ui->openGLWidget->drawShape(f_robot_status_data.ROV_IMU_data.pitch,f_robot_status_data.ROV_IMU_data.roll, 0);
    ui->openGLWidget_yaw->drawShape(0,0,f_robot_status_data.ROV_IMU_data.yaw);

    ui->ax->setText(QString::number(f_robot_status_data.ROV_IMU_data.ax,'f',2));
    ui->ay->setText(QString::number(f_robot_status_data.ROV_IMU_data.ay,'f',2));
    ui->az->setText(QString::number(f_robot_status_data.ROV_IMU_data.az,'f',2));
    ui->gx->setText(QString::number(f_robot_status_data.ROV_IMU_data.gx,'f',2));
    ui->gy->setText(QString::number(f_robot_status_data.ROV_IMU_data.gy,'f',2));
    ui->gz->setText(QString::number(f_robot_status_data.ROV_IMU_data.gz,'f',2));

    ui->cabin_temp->setText(QString::number(f_robot_status_data.ROV_cabin_status.cabin_temperature,'f',2));
    ui->cabin_humi->setText(QString::number(f_robot_status_data.ROV_cabin_status.cabin_humidity,'f',2));
    ui->cabin_humi_2->setText(QString::number(f_robot_status_data.ROV_cabin_status.cabin_water_level,'f',2));
    ui->cpu_temp->setText(QString::number(f_robot_status_data.ROV_cabin_status.cpu_temperature,'f',2));
    ui->altitude->setText(QString::number(f_robot_status_data.ROV_depth,'f',2));
    ui->height->setText(QString::number(f_robot_status_data.ROV_height,'f',2));
    update_status(f_robot_status_data.ROV_run_mode);
}

// ROV 运动状态更新
void MainWindow::update_status(uint8_t run_mode)
{
    switch (run_mode) {
    case 0 :
        ui->status_label->setText("无力模式");
        break;
    case 1 :
        ui->status_label->setText("开环模式");
        break;
    case 2 :
        ui->status_label->setText("只开定深");
        break;
    case 3 :
        ui->status_label->setText("只开定艏");
        break;
    case 4 :
        ui->status_label->setText("定深定艏都开");
        break;
    case 5 :
        ui->status_label->setText("贴壁爬行（竖推工作）");
        break;
    case 6 :
        ui->status_label->setText("陆地爬行（竖推不工作）");
        break;
    case 7 :
        ui->status_label->setText("停止模式，稳定当前状态，进行作业（暂不开发）");
        break;

    }

}

//处理ROV上传的文本信息
void MainWindow::process_ROV_txtmsg(char* msg)
{

    QVector<QColor> colors = {
        Qt::red, Qt::green, Qt::black,Qt::blue,Qt::darkRed,Qt::cyan,Qt::magenta
    };
    int index = QRandomGenerator::global()->bounded(colors.size());
    ui->txt_msg->setTextColor(colors.at(index));

    QString txt = msg;
    ui->txt_msg->append(QTime::currentTime().toString("HH:mm:ss:zzz")+"--ROV say: ");
    ui->txt_msg->append(txt);

    memset(msg,0,200);
}

//------------ui按钮------------------------------
void MainWindow::on_lightON_clicked()
{
    light_on = 1;
    ui->lightON->setEnabled(false);
    ui->lightOFF->setEnabled(true);
    emit s_brightness(ui->brightness->value());
    ui->brightness_label->setText(QString::number(ui->brightness->value()));
}

void MainWindow::on_lightOFF_clicked()
{
    light_on = 0;
    ui->lightON->setEnabled(true);
    ui->lightOFF->setEnabled(false);
    emit s_brightness(0);
    ui->brightness_label->setText("0");
}

void MainWindow::on_brightness_sliderReleased()
{
    if(light_on){
        emit s_brightness(ui->brightness->value());
        ui->brightness_label->setText(QString::number(ui->brightness->value()));
    }
}

// 滑动条移动改变舵机角度
void MainWindow::on_steering_gear_angle_sliderReleased()
{
    ui->steering_gear_angle_label->setText(QString::number(ui->steering_gear_angle->value()));
    emit s_servo_angle((int8_t)(ui->steering_gear_angle->value()));
}

// “舵机前移”按钮改变舵机角度
void MainWindow::on_servo_angle_up_clicked()
{
    ui->steering_gear_angle->setValue(ui->steering_gear_angle->value()+1);
    ui->steering_gear_angle_label->setText(QString::number(ui->steering_gear_angle->value()));
    emit s_servo_angle((int8_t)(ui->steering_gear_angle->value()));
}
void MainWindow::on_servo_angle_down_clicked()
{
    ui->steering_gear_angle->setValue(ui->steering_gear_angle->value()-1);
    ui->steering_gear_angle_label->setText(QString::number(ui->steering_gear_angle->value()));
    emit s_servo_angle((int8_t)(ui->steering_gear_angle->value()));
}


void MainWindow::on_action_radioButton_clicked()
{
    QMessageBox::information(this,"运动档提醒","当前进入运动档，请谨慎开车！");
    speed_adjust = SPEED_ADJUST_ACTION;
}

void MainWindow::on_common_radioButton_clicked()
{
    QMessageBox::information(this,"普通档提醒","当前进入普通档，驾驶速度减慢");
    speed_adjust = SPEED_ADJUST_COMMON;
}

void MainWindow::on_HDvideo_clicked()
{
    zhifan * my_zhifan = new(zhifan);
    my_zhifan->show();
}


void MainWindow::open_worker1_connects()
{
    //手柄信息 -> 端口1 发送消息
    connect(m_gamepad, &QGamepad::buttonAChanged,worker,&sendfile::sendMsgA);
    connect(m_gamepad, &QGamepad::buttonBChanged,worker,&sendfile::sendMsgB);
    connect(m_gamepad, &QGamepad::buttonXChanged,worker,&sendfile::sendMsgX);
    void (sendfile:: * sendMsgYbool)(bool) = &sendfile::sendMsgY;//解决槽函数中sendMsgY函数重载，创建一个函数指针用于保存指定的函数地址
    void (sendfile:: * sendMsgUpbool)(bool) = &sendfile::sendMsgUp;//解决槽函数中sendMsgY函数重载，创建一个函数指针用于保存指定的函数地址
    connect(m_gamepad, &QGamepad::buttonYChanged,worker,sendMsgYbool);
    connect(m_gamepad, &QGamepad::buttonUpChanged,worker,sendMsgUpbool);
    connect(m_gamepad, &QGamepad::buttonLeftChanged,worker,&sendfile::sendMsgLeft);
    connect(m_gamepad, &QGamepad::buttonL1Changed,worker,&sendfile::sendMsgL1);
    connect(m_gamepad, &QGamepad::buttonR1Changed,worker,&sendfile::sendMsgR1);
    connect(m_gamepad, &QGamepad::buttonL2Changed,worker,&sendfile::sendMsgL2);
    connect(m_gamepad, &QGamepad::buttonR2Changed,worker,&sendfile::sendMsgR2);
    connect(m_gamepad, &QGamepad::axisLeftXChanged,worker,&sendfile::sendMsgLeftX);
    connect(m_gamepad, &QGamepad::axisLeftYChanged,worker,&sendfile::sendMsgLeftY);
    connect(m_gamepad, &QGamepad::axisRightXChanged,worker,&sendfile::sendMsgRightX);
    connect(m_gamepad, &QGamepad::axisRightYChanged,worker,&sendfile::sendMsgRightY);
    //界面按钮-> 端口1 发送消息
    void (sendfile:: * sendMsgY)() = &sendfile::sendMsgY;//解决槽函数中sendMsgY函数重载，创建一个函数指针用于保存指定的函数地址
    void (sendfile:: * sendMsgUp)() = &sendfile::sendMsgUp;//解决槽函数中sendMsgY函数重载，创建一个函数指针用于保存指定的函数地址
    connect(ui->AltHoldModeON, &QAbstractButton::clicked,worker,sendMsgY);
    connect(ui->AltHoldModeOFF, &QToolButton::clicked,worker,sendMsgUp);
    connect(this,&MainWindow::s_brightness,worker,&sendfile::sendMsgBrightnessSet);//设置灯的亮度
    connect(this,&MainWindow::s_servo_angle,worker,&sendfile::sendMsgServoAngle);//设置舵机角度
}

void MainWindow::close_worker1_connects()
{
    //手柄信息 -> 端口1 发送消息
    disconnect(m_gamepad, &QGamepad::buttonAChanged,worker,&sendfile::sendMsgA);
    disconnect(m_gamepad, &QGamepad::buttonBChanged,worker,&sendfile::sendMsgB);
    disconnect(m_gamepad, &QGamepad::buttonXChanged,worker,&sendfile::sendMsgX);
    void (sendfile:: * sendMsgYbool)(bool) = &sendfile::sendMsgY;//解决槽函数中sendMsgY函数重载，创建一个函数指针用于保存指定的函数地址
    void (sendfile:: * sendMsgUpbool)(bool) = &sendfile::sendMsgUp;//解决槽函数中sendMsgY函数重载，创建一个函数指针用于保存指定的函数地址
    disconnect(m_gamepad, &QGamepad::buttonYChanged,worker,sendMsgYbool);
    disconnect(m_gamepad, &QGamepad::buttonUpChanged,worker,sendMsgUpbool);
    disconnect(m_gamepad, &QGamepad::buttonLeftChanged,worker,&sendfile::sendMsgLeft);
    disconnect(m_gamepad, &QGamepad::buttonL1Changed,worker,&sendfile::sendMsgL1);
    disconnect(m_gamepad, &QGamepad::buttonR1Changed,worker,&sendfile::sendMsgR1);
    disconnect(m_gamepad, &QGamepad::buttonL2Changed,worker,&sendfile::sendMsgL2);
    disconnect(m_gamepad, &QGamepad::buttonR2Changed,worker,&sendfile::sendMsgR2);
    disconnect(m_gamepad, &QGamepad::axisLeftXChanged,worker,&sendfile::sendMsgLeftX);
    disconnect(m_gamepad, &QGamepad::axisLeftYChanged,worker,&sendfile::sendMsgLeftY);
    disconnect(m_gamepad, &QGamepad::axisRightXChanged,worker,&sendfile::sendMsgRightX);
    disconnect(m_gamepad, &QGamepad::axisRightYChanged,worker,&sendfile::sendMsgRightY);

    //界面按钮-> 端口1 发送消息
    void (sendfile:: * sendMsgY)() = &sendfile::sendMsgY;//解决槽函数中sendMsgY函数重载，创建一个函数指针用于保存指定的函数地址
    void (sendfile:: * sendMsgUp)() = &sendfile::sendMsgUp;//解决槽函数中sendMsgY函数重载，创建一个函数指针用于保存指定的函数地址
    disconnect(ui->AltHoldModeON, &QAbstractButton::clicked,worker,sendMsgY);
    disconnect(ui->AltHoldModeOFF, &QToolButton::clicked,worker,sendMsgUp);
    disconnect(this,&MainWindow::s_brightness,worker,&sendfile::sendMsgBrightnessSet);//设置灯的亮度
    disconnect(this,&MainWindow::s_servo_angle,worker,&sendfile::sendMsgServoAngle);//设置舵机角度
}


//--------------------------------------------
//-------------摄像头---------------------------

void MainWindow::on_cameraON_clicked()
{
    // 使用水下摄像头则使用以下固定的 RTSP码流格式
    // rtsp://192.168.1.168:8554/0
    // rtmp://192.168.1.168:1935/live/ls_00_0


    QString url1 = camera1_urls.trimmed();
    ui->camera->setUrl(url1);
    ui->camera->open();

    ui->cameraON->setEnabled(false);
    ui->cameraOFF->setEnabled(true);

}

void MainWindow::on_cameraOFF_clicked()
{
    ui->camera->close();
    ui->cameraON->setEnabled(true);
    ui->cameraOFF->setEnabled(false);   
//    ui->camera->set_default_picture(QImage(":/images/images/camera1.png"));
}

void MainWindow::on_camera_detection_ON_clicked()
{
    QString url2 = camera2_urls.trimmed();
    ui->camera_2->setUrl(url2);
    ui->camera_2->open();

    ui->camera_detection_ON->setEnabled(false);
    ui->camera_detection_OFF->setEnabled(true);
}

void MainWindow::on_camera_detection_OFF_clicked()
{
    ui->camera_2->close();
    ui->camera_detection_ON->setEnabled(true);
    ui->camera_detection_OFF->setEnabled(false);
//    ui->camera_2->set_default_picture(QImage(":/images/images/camera2.png"));
}


void MainWindow::on_videoRecodeON_clicked()
{
//    ui->camera->videoRecodeOn();

//    ui->videoRecodeON->setEnabled(false);
//    ui->videoRecodeOFF->setEnabled(true);
}

void MainWindow::on_videoRecodeOFF_clicked()
{
//     ui->camera->videoRecodeOFF();

//     ui->videoRecodeOFF->setEnabled(false);
//     ui->videoRecodeON->setEnabled(true);
}


void MainWindow::on_pictureRecode_clicked()
{
    ui->camera->takeOnePhoto();
}

void MainWindow::on_camera_detection_recode_clicked()
{
    ui->camera_2->takeOnePhoto();
}

//-------------摄像头---------------------------
//---------------------------------------------



//--------------------------------------------
//-------------声纳---------------------------
void MainWindow::on_sonar_ON_clicked()
{
    ui->sonar->start_sonar();
    ui->sonar_ON->setEnabled(false);
    ui->sonar_OFF->setEnabled(true);
}

void MainWindow::on_sonar_OFF_clicked()
{
    ui->sonar->stop_sonar();
    ui->sonar_ON->setEnabled(true);
    ui->sonar_OFF->setEnabled(false);
    ui->sonar->set_default_picture(QImage(":/images/images/sonar.png"));
}




//--------------------------------------------
//-------------页面显示切换---------------------------
void MainWindow::on_sonar_is_main_button_clicked()
{
    if(camera_status == 0){
        ui->aspect_ratio_weight_1->set_layout_remove(ui->camera);
        ui->aspect_ratio_weight_2->set_layout_remove(ui->camera_2);
        ui->aspect_ratio_weight_3->set_layout_remove(ui->sonar);

        ui->aspect_ratio_weight_1->set_ration(8,7);
        ui->aspect_ratio_weight_1->set_layout(ui->sonar);
        ui->aspect_ratio_weight_2->set_ration(16,9);
        ui->aspect_ratio_weight_2->set_layout(ui->camera);
        ui->aspect_ratio_weight_3->set_ration(16,9);
        ui->aspect_ratio_weight_3->set_layout(ui->camera_2);
    }
    else if(camera_status == 1){
        ui->aspect_ratio_weight_1->set_layout_remove(ui->camera_2);
        ui->aspect_ratio_weight_2->set_layout_remove(ui->camera);
        ui->aspect_ratio_weight_3->set_layout_remove(ui->sonar);

        ui->aspect_ratio_weight_1->set_ration(8,7);
        ui->aspect_ratio_weight_1->set_layout(ui->sonar);
        ui->aspect_ratio_weight_2->set_ration(16,9);
        ui->aspect_ratio_weight_2->set_layout(ui->camera);
        ui->aspect_ratio_weight_3->set_ration(16,9);
        ui->aspect_ratio_weight_3->set_layout(ui->camera_2);
    }
    else if(camera_status == 2){
    }
    ui->camera_is_main_button->setEnabled(true);
    ui->camera2_is_main_button->setEnabled(true);
    ui->sonar_is_main_button->setEnabled(false);
    camera_status = 2;
}

void MainWindow::on_camera_is_main_button_clicked()
{
    if(camera_status == 0){
    }
    else if(camera_status == 1){
        ui->aspect_ratio_weight_1->set_layout_remove(ui->camera_2);
        ui->aspect_ratio_weight_2->set_layout_remove(ui->camera);
        ui->aspect_ratio_weight_3->set_layout_remove(ui->sonar);

        ui->aspect_ratio_weight_1->set_ration(16,9);
        ui->aspect_ratio_weight_1->set_layout(ui->camera);
        ui->aspect_ratio_weight_2->set_ration(16,9);
        ui->aspect_ratio_weight_2->set_layout(ui->camera_2);
        ui->aspect_ratio_weight_3->set_ration(8,7);
        ui->aspect_ratio_weight_3->set_layout(ui->sonar);

    }
    else if(camera_status == 2){
        ui->aspect_ratio_weight_1->set_layout_remove(ui->sonar);
        ui->aspect_ratio_weight_2->set_layout_remove(ui->camera);
        ui->aspect_ratio_weight_3->set_layout_remove(ui->camera_2);

        ui->aspect_ratio_weight_1->set_ration(16,9);
        ui->aspect_ratio_weight_1->set_layout(ui->camera);
        ui->aspect_ratio_weight_2->set_ration(16,9);
        ui->aspect_ratio_weight_2->set_layout(ui->camera_2);
        ui->aspect_ratio_weight_3->set_ration(8,7);
        ui->aspect_ratio_weight_3->set_layout(ui->sonar);

    }

    ui->camera2_is_main_button->setEnabled(true);
    ui->sonar_is_main_button->setEnabled(true);
    ui->camera_is_main_button->setEnabled(false);
    camera_status = 0;
}

void MainWindow::on_camera2_is_main_button_clicked()
{
    if(camera_status == 0){
        ui->aspect_ratio_weight_1->set_layout_remove(ui->camera);
        ui->aspect_ratio_weight_2->set_layout_remove(ui->camera_2);
        ui->aspect_ratio_weight_3->set_layout_remove(ui->sonar);

        ui->aspect_ratio_weight_1->set_ration(16,9);
        ui->aspect_ratio_weight_1->set_layout(ui->camera_2);
        ui->aspect_ratio_weight_2->set_ration(16,9);
        ui->aspect_ratio_weight_2->set_layout(ui->camera);
        ui->aspect_ratio_weight_3->set_ration(8,7);
        ui->aspect_ratio_weight_3->set_layout(ui->sonar);
    }
    else if(camera_status == 1){

    }
    else if(camera_status == 2){
        ui->aspect_ratio_weight_1->set_layout_remove(ui->sonar);
        ui->aspect_ratio_weight_2->set_layout_remove(ui->camera);
        ui->aspect_ratio_weight_3->set_layout_remove(ui->camera_2);

        ui->aspect_ratio_weight_1->set_ration(16,9);
        ui->aspect_ratio_weight_1->set_layout(ui->camera_2);
        ui->aspect_ratio_weight_2->set_ration(16,9);
        ui->aspect_ratio_weight_2->set_layout(ui->camera);
        ui->aspect_ratio_weight_3->set_ration(8,7);
        ui->aspect_ratio_weight_3->set_layout(ui->sonar);
    }

    ui->camera_is_main_button->setEnabled(true);
    ui->sonar_is_main_button->setEnabled(true);
    ui->camera2_is_main_button->setEnabled(false);

    camera_status = 1;
}


