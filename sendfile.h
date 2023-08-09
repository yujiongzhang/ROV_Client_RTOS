#ifndef SENDFILE_H
#define SENDFILE_H

#include <QObject>
#include <QTcpSocket>
#include "msg.h"
#include "crc8_crc16.h"
#include <QMetaType>


static ROVComuPacket ROV2PCComu; //创建接收ROV数据上传的数据包
static ROVComuPacket PC2ROVComu; //创建下发指令的数据包
static struct Robot_status_DATA robot_status_data; //表示机器人状态的DATA中的结构体
static struct motion_control_cmd_DATA motion_control_cmd_data; //下发运动指令的DATA中的结构体,并初始化
static struct target_control_cmd_DATA target_control_cmd_data; //下发控制目标指令的结构体

static int speed_adjust;


class sendfile : public QObject
{
    Q_OBJECT
public:
    explicit sendfile(QObject *parent = nullptr);

    // 连接服务器
    void connectServer(unsigned short port, QString ip);

    //发送文件
    void sendFile(QString path);

    //发送信息
    void sendMsg(QString msg);

    //发送手柄信号到socket
    void sendMsgA(bool pressed);
    void sendMsgB(bool pressed);
    void sendMsgX(bool pressed);
    void sendMsgY(bool pressed);
    void sendMsgUp(bool pressed);
    void sendMsgLeft(bool pressed);
    void sendMsgL1(bool pressed);
    void sendMsgR1(bool pressed);
    void sendMsgL2(bool pressed);
    void sendMsgR2(bool pressed);
    void sendMsgLeftX(double value);
    void sendMsgLeftY(double value);
    void sendMsgRightX(double value);
    void sendMsgRightY(double value);

    //ui按键信号到socket
    void sendMsgY();
    void sendMsgUp();
    void sendMsgBrightnessSet(int value);
    void sendMsgServoAngle(int8_t value);
    void sendMsgTargetDepth(float value);

    void sendMsgPID(PIDs_set_DATA value);

    //包装 PC2ROVComu 的函数，传入消息类型，DATA的指针（首地址），DATA的长度
    void set_PC2ROVComu(uint8_t msg_type, char* dataPtr,int datalen);
    //往 socket 中写入 PC2ROVComu 结构体。参数：消息类型
    void send_TcpData(uint8_t msg_type);

    //从 socket 中读取ROV2PCComu 结构体
    void rev_TcpData();
    void process_TcpData_type1();//从处理类型1的消息
    void process_TcpData_type2();//从处理类型2的消息


signals:
    void connectOK();
    void gameover();
    void curPercent(int num);

    //接收信息
    void recvMsg(QByteArray msg);
    void recvAngle(msg_attitude msg);

    void s_ROV_status(Robot_status_DATA msg);
    void s_ROV_txtmsg(char* msg);

private:
    QTcpSocket * m_tcp;
    char rov_msg_str[200]; //记录rov上传的文本信息;


};

#endif // SENDFILE_H
