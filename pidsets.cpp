// author: zyj
// File: pidsets.cpp

#include "pidsets.h"
#include "ui_pidsets.h"
#include<QtDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

PIDSets::PIDSets(QWidget *parent) :
    QMainWindow(parent,Qt::Widget | Qt::WindowStaysOnTopHint),
    ui(new Ui::PIDSets)
{
    ui->setupUi(this);
    qDebug() << "创建 PID 类" ;
    this->setAttribute(Qt::WA_DeleteOnClose,true);    //关闭窗口即delete该类
    qRegisterMetaType<PIDs_set_DATA>("PIDs_set_DATA");//在跨线程的信号和槽的参数传递中, 参数的类型是自定义的类型，需要注册一下
    this->PID_set_init();

}

PIDSets::~PIDSets()
{
    delete ui;
    qDebug() << "PIDsets类释放" ;
}

void PIDSets::PID_set_init()
{
    ui->P1->setText("0");
    ui->I1->setText("0");
    ui->D1->setText("0");
    ui->P2->setText("0");
    ui->I2->setText("0");
    ui->D2->setText("0");
    ui->P3->setText("0");
    ui->I3->setText("0");
    ui->D3->setText("0");
    ui->P4->setText("0");
    ui->I4->setText("0");
    ui->D4->setText("0");

    ui->P5->setText("0");
    ui->I5->setText("0");
    ui->D5->setText("0");
    ui->P6->setText("0");
    ui->I6->setText("0");
    ui->D6->setText("0");
    ui->P7->setText("0");
    ui->I7->setText("0");
    ui->D7->setText("0");
    ui->P8->setText("0");
    ui->I8->setText("0");
    ui->D8->setText("0");

    ui->P9->setText("0");
    ui->I9->setText("0");
    ui->D9->setText("0");

    pids_set_data = {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};

}

void PIDSets::import_pid_config(QString _file)
{
    QFile file(_file);
    file.open(QFile::ReadOnly);
    QByteArray all = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(all);

    if(doc.isObject())
        {
            QJsonObject obj = doc.object();
            ui->P1->setText(obj.value("P1").toString());
            ui->I1->setText(obj.value("I1").toString());
            ui->D1->setText(obj.value("D1").toString());
            ui->P2->setText(obj.value("P2").toString());
            ui->I2->setText(obj.value("I2").toString());
            ui->D2->setText(obj.value("D2").toString());
            ui->P3->setText(obj.value("P3").toString());
            ui->I3->setText(obj.value("I3").toString());
            ui->D3->setText(obj.value("D3").toString());
            ui->P4->setText(obj.value("P4").toString());
            ui->I4->setText(obj.value("I4").toString());
            ui->D4->setText(obj.value("D4").toString());

            ui->P5->setText(obj.value("P5").toString());
            ui->I5->setText(obj.value("I5").toString());
            ui->D5->setText(obj.value("D5").toString());
            ui->P6->setText(obj.value("P6").toString());
            ui->I6->setText(obj.value("I6").toString());
            ui->D6->setText(obj.value("D6").toString());
            ui->P7->setText(obj.value("P7").toString());
            ui->I7->setText(obj.value("I7").toString());
            ui->D7->setText(obj.value("D7").toString());
            ui->P8->setText(obj.value("P8").toString());
            ui->I8->setText(obj.value("I8").toString());
            ui->D8->setText(obj.value("D8").toString());

            ui->P9->setText(obj.value("P9").toString());
            ui->I9->setText(obj.value("I9").toString());
            ui->D9->setText(obj.value("D9").toString());
        }
    else {
        QMessageBox::information(this,"导入错误","找不到对应配置文件");
    }
}

void PIDSets::save_pid_config(QString _file)
{
    QFile file(_file);
    file.open(QFile::ReadOnly);
    QByteArray all = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(all);

    if(doc.isObject())
    {
            QJsonObject obj = doc.object();
            obj["P1"] = ui->P1->text();
            obj["I1"] = ui->I1->text();
            obj["D1"] = ui->D1->text();
            obj["P2"] = ui->P2->text();
            obj["I2"] = ui->I2->text();
            obj["D2"] = ui->D2->text();
            obj["P3"] = ui->P3->text();
            obj["I3"] = ui->I3->text();
            obj["D3"] = ui->D3->text();
            obj["P4"] = ui->P4->text();
            obj["I4"] = ui->I4->text();
            obj["D4"] = ui->D4->text();
            obj["P5"] = ui->P5->text();
            obj["I5"] = ui->I5->text();
            obj["D5"] = ui->D5->text();
            obj["P6"] = ui->P6->text();
            obj["I6"] = ui->I6->text();
            obj["D6"] = ui->D6->text();
            obj["P7"] = ui->P7->text();
            obj["I7"] = ui->I7->text();
            obj["D7"] = ui->D7->text();
            obj["P8"] = ui->P8->text();
            obj["I8"] = ui->I8->text();
            obj["D8"] = ui->D8->text();
            obj["P9"] = ui->P9->text();
            obj["I9"] = ui->I9->text();
            obj["D9"] = ui->D9->text();
            QJsonDocument newdoc(obj);
            QByteArray json = newdoc.toJson();
            QFile newfile(_file);
            newfile.open(QFile::WriteOnly);
            newfile.write(json);
            newfile.close();
            QMessageBox::information(this,"保存配置","配置文件已保存");
    }
    else {
        QMessageBox::information(this,"导入错误","找不到对应配置文件");
    }
}



void PIDSets::on_PID_set_button_clicked()
{
    pids_set_data.depth_loop_pid.P_set = ui->P1->text().toInt();
    pids_set_data.depth_loop_pid.I_set = ui->I1->text().toInt();
    pids_set_data.depth_loop_pid.D_set = ui->D1->text().toInt();

    pids_set_data.track_speed_pid.P_set = ui->P2->text().toInt();
    pids_set_data.track_speed_pid.I_set = ui->I2->text().toInt();
    pids_set_data.track_speed_pid.D_set = ui->D2->text().toInt();

    pids_set_data.track_turn_loop_pid.P_set = ui->P3->text().toInt();
    pids_set_data.track_turn_loop_pid.I_set = ui->I3->text().toInt();
    pids_set_data.track_turn_loop_pid.D_set = ui->D3->text().toInt();

    pids_set_data.yaw_angle_pid.P_set = ui->P4->text().toInt();
    pids_set_data.yaw_angle_pid.I_set = ui->I4->text().toInt();
    pids_set_data.yaw_angle_pid.D_set = ui->D4->text().toInt();

    pids_set_data.yaw_angular_velocity_pid.P_set = ui->P5->text().toInt();
    pids_set_data.yaw_angular_velocity_pid.I_set = ui->I5->text().toInt();
    pids_set_data.yaw_angular_velocity_pid.D_set = ui->D5->text().toInt();

    pids_set_data.pitch_angle_pid.P_set = ui->P6->text().toInt();
    pids_set_data.pitch_angle_pid.I_set = ui->I6->text().toInt();
    pids_set_data.pitch_angle_pid.D_set = ui->D6->text().toInt();

    pids_set_data.pitch_angular_velocity_pid.P_set = ui->P7->text().toInt();
    pids_set_data.pitch_angular_velocity_pid.I_set = ui->I7->text().toInt();
    pids_set_data.pitch_angular_velocity_pid.D_set = ui->D7->text().toInt();

    pids_set_data.roll_angle_pid.P_set = ui->P8->text().toInt();
    pids_set_data.roll_angle_pid.I_set = ui->I8->text().toInt();
    pids_set_data.roll_angle_pid.D_set = ui->D8->text().toInt();

    pids_set_data.roll_angular_velocity_pid.P_set = ui->P9->text().toInt();
    pids_set_data.roll_angular_velocity_pid.I_set = ui->I9->text().toInt();
    pids_set_data.roll_angular_velocity_pid.D_set = ui->D9->text().toInt();

    emit s_pidset(pids_set_data);
}

void PIDSets::on_PID_zero_set_button_clicked()
{
    this->PID_set_init();
}

//---导入PID 配置文件-----------
void PIDSets::on_import_config1_pushButton_clicked()
{
    QString config_file = "./config/pidConfig1.json";
    import_pid_config(config_file);
}
void PIDSets::on_import_config2_pushButton_clicked()
{
    QString config_file = "./config/pidConfig2.json";
    import_pid_config(config_file);
}
void PIDSets::on_import_config3_pushButton_clicked()
{
    QString config_file = "./config/pidConfig3.json";
    import_pid_config(config_file);
}
void PIDSets::on_import_config4_pushButton_clicked()
{
    QString config_file = "./config/pidConfig4.json";
    import_pid_config(config_file);
}

//---保存PID 配置文件-----------
void PIDSets::on_save_config1_pushButton_clicked()
{
    QString config_file = "./config/pidConfig1.json";
    save_pid_config(config_file);
}
void PIDSets::on_save_config2_pushButton_clicked()
{
    QString config_file = "./config/pidConfig2.json";
    save_pid_config(config_file);
}
void PIDSets::on_save_config3_pushButton_clicked()
{
    QString config_file = "./config/pidConfig3.json";
    save_pid_config(config_file);
}
void PIDSets::on_save_config4_pushButton_clicked()
{
    QString config_file = "./config/pidConfig4.json";
    save_pid_config(config_file);
}
