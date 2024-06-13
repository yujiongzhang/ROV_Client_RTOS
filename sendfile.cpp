#include "sendfile.h"

#include <QFile>
#include <QFileInfo>
#include <QHostAddress>
#include <qthread.h>

#include <WinSock2.h>

class RobotComuPacket msg_recv;

QByteArray sendTcpData;
QByteArray recvTcpData;
QByteArray txt_msg;

sendfile::sendfile(QObject *parent) : QObject(parent)
{
    PC2ROVComu.header = 0xA5;
    motion_control_cmd_data = {0,0,0,0,0,0,0,0,0};
    target_control_cmd_data = {0};
    qRegisterMetaType<msg_attitude>("msg_attitude");//跨线程的信号和槽的参数传递中,参数的类型是自定义的类型
    qRegisterMetaType<Robot_status_DATA>("Robot_status_DATA");//在跨线程的信号和槽的参数传递中, 参数的类型是自定义的类型，需要注册一下
}

void sendfile::connectServer(unsigned short port, QString ip)
{
    qDebug()<<"当前子线程的ID："<< QThread::currentThread();
    m_tcp = new QTcpSocket;
    m_tcp->connectToHost(QHostAddress(ip),port);

    connect(m_tcp,&QTcpSocket::connected,this,&sendfile::connectOK);
    connect(m_tcp,&QTcpSocket::disconnected,this,[=](){
        m_tcp->close();
        m_tcp->deleteLater();
        emit gameover();
    });

    //检测是否可以接受数据
    connect(m_tcp, &QTcpSocket::readyRead,this,&sendfile::rev_TcpData);

}

void sendfile::sendFile(QString path)
{
    QFile file(path);
    QFileInfo info(path);
    int fileSize = info.size();
    file.open(QFile::ReadOnly);
    while (!file.atEnd())
    {
        static int num = 0;
        if(num==0){
            m_tcp->write((char*)&fileSize,4);
        }
        QByteArray line = file.readLine();
        num += line.size();
        int percent = (num*100/fileSize);
        emit curPercent(percent);

        m_tcp->write(line);
    }
}

void sendfile::sendfile::sendMsg(QString msg)
{
    m_tcp->write(msg.toUtf8());
}

//----------------------------------------------------------------
// ---------------------- 手柄指令 -> ROV -------------------------
//----------------------------------------------------------------

void sendfile::sendMsgA(bool pressed)
{
    if(pressed){
        qDebug() << "sendMsgA线程地址: " << QThread::currentThread();
        qDebug() << "Button A" << pressed;
        QString msg = "Button A";
        m_tcp->write(msg.toUtf8());}
}

void sendfile::sendMsgB(bool pressed)
{
    if(pressed){
        qDebug() << "Button B" << pressed;

        if(motion_control_cmd_data.ModeType == 0 || motion_control_cmd_data.ModeType == 1 ||
           motion_control_cmd_data.ModeType == 2 ||
           motion_control_cmd_data.ModeType == 4 || motion_control_cmd_data.ModeType == 5)
        {
            motion_control_cmd_data.ModeType = 3; //按下按钮B后开启定航模式
         }


        //包装 PC2ROVComu 的函数，传入消息类型，DATA的指针（首地址），DATA的长度
        set_PC2ROVComu(1,(char*)(&motion_control_cmd_data),sizeof(motion_control_cmd_data));
        send_TcpData(1);//发送消息类型为1

    }

}

void sendfile::sendMsgLeft(bool pressed)
{
    if(pressed){
        qDebug() << "Button Left" << pressed;

        if(motion_control_cmd_data.ModeType == 0 || motion_control_cmd_data.ModeType == 1 ||
           motion_control_cmd_data.ModeType == 2 ||
           motion_control_cmd_data.ModeType == 3 || motion_control_cmd_data.ModeType == 5)
        {
            motion_control_cmd_data.ModeType = 4; //按下按钮Left后开启定航模式
         }

        set_PC2ROVComu(1,(char*)(&motion_control_cmd_data),sizeof(motion_control_cmd_data));
        send_TcpData(1);//发送消息类型为1
    }
}

void sendfile::sendMsgX(bool pressed)
{
    if(pressed)
    {
        qDebug() << "Button X" << pressed;

    }
}

void sendfile::sendMsgY(bool pressed)
{
    if(pressed){
        qDebug() << "Button Y" << pressed;

        if(motion_control_cmd_data.ModeType == 0 || motion_control_cmd_data.ModeType == 1 ||
           motion_control_cmd_data.ModeType == 3 ||
           motion_control_cmd_data.ModeType == 4 || motion_control_cmd_data.ModeType == 5)
        {
            motion_control_cmd_data.ModeType = 2; //按下按钮Y后开启定深模式
         }

        set_PC2ROVComu(1,(char*)(&motion_control_cmd_data),sizeof(motion_control_cmd_data));
        send_TcpData(1);//发送消息类型为1
    }
}

void sendfile::sendMsgUp(bool pressed)
{
    if(pressed){
        qDebug() << "Button Up" << pressed;

        if(motion_control_cmd_data.ModeType == 0 || motion_control_cmd_data.ModeType == 2 ||
           motion_control_cmd_data.ModeType == 3 ||
           motion_control_cmd_data.ModeType == 4 || motion_control_cmd_data.ModeType == 5)
        {
            motion_control_cmd_data.ModeType = 1; //按下按钮UP后为 开环模式
         }

        set_PC2ROVComu(1,(char*)(&motion_control_cmd_data),sizeof(motion_control_cmd_data));
        send_TcpData(1);//发送消息类型为1
    }
}

void sendfile::sendMsgL1(bool pressed)
{
    if(pressed){
        qDebug() << "Button L1" << pressed;

        if(motion_control_cmd_data.ModeType == 0 )
        {
            motion_control_cmd_data.ModeType = 6; //按下按钮L1后为 陆地爬行模式
         }

        set_PC2ROVComu(1,(char*)(&motion_control_cmd_data),sizeof(motion_control_cmd_data));
        send_TcpData(1);//发送消息类型为1
    }
}

void sendfile::sendMsgR1(bool pressed)
{
    if(pressed){
        qDebug() << "Button T1" << pressed;

        if(motion_control_cmd_data.ModeType == 1 || motion_control_cmd_data.ModeType == 2 ||
           motion_control_cmd_data.ModeType == 3 ||
           motion_control_cmd_data.ModeType == 4 )
        {
            motion_control_cmd_data.ModeType = 5; //按下按钮R1后为 爬壁模式
         }
        set_PC2ROVComu(1,(char*)(&motion_control_cmd_data),sizeof(motion_control_cmd_data));
        send_TcpData(1);//发送消息类型为1
    }
}

void sendfile::sendMsgL2(bool pressed)
{
    if(pressed){
        qDebug() << "Button L2" << pressed;

        motion_control_cmd_data.ModeType = 0; //按下按钮L1后为 无力模式

        set_PC2ROVComu(1,(char*)(&motion_control_cmd_data),sizeof(motion_control_cmd_data));
        send_TcpData(1);//发送消息类型为1
    }
}

void sendfile::sendMsgR2(bool pressed)
{
    if(pressed){
        qDebug() << "Button L2" << pressed;

        motion_control_cmd_data.ModeType = 7; //按下按钮L1后为 停止

        set_PC2ROVComu(1,(char*)(&motion_control_cmd_data),sizeof(motion_control_cmd_data));
        send_TcpData(1);//发送消息类型为1
    }
}


void sendfile::sendMsgLeftX(double value)
{
    qDebug() << "Left X" << value;

    motion_control_cmd_data.ROL = int(value*180);

    set_PC2ROVComu(1,(char*)(&motion_control_cmd_data),sizeof(motion_control_cmd_data));
    send_TcpData(1);//发送消息类型为1
}

void sendfile::sendMsgLeftY(double value)
{
    qDebug() << "Left Y" << value;

//    if(speed_adjust == SPEED_ADJUST_COMMON){
//        motion_control_cmd_data.VZ = int(value*60);
//    }
//    else if(speed_adjust == SPEED_ADJUST_ACTION)
//    {
        motion_control_cmd_data.VZ = int(value*100);
//    }

    set_PC2ROVComu(1,(char*)(&motion_control_cmd_data),sizeof(motion_control_cmd_data));
    send_TcpData(1);//发送消息类型为1
}

void sendfile::sendMsgRightX(double value)
{
    qDebug() << "Righ tX" << value;

    motion_control_cmd_data.YAW = int(value*180);

    set_PC2ROVComu(1,(char*)(&motion_control_cmd_data),sizeof(motion_control_cmd_data));
    send_TcpData(1);//发送消息类型为1
}

void sendfile::sendMsgRightY(double value)
{
//    if(speed_adjust == SPEED_ADJUST_COMMON){
//        motion_control_cmd_data.VX = int(-value*60);
//    }
//    else if(speed_adjust == SPEED_ADJUST_ACTION){
        motion_control_cmd_data.VX = int(-value*100);
//    }

    set_PC2ROVComu(1,(char*)(&motion_control_cmd_data),sizeof(motion_control_cmd_data));
    send_TcpData(1);//发送消息类型为1
}


void sendfile::sendMsgY()
{

    qDebug() <<  "Button AltHOldModeON";

    if(motion_control_cmd_data.ModeType == 0 || motion_control_cmd_data.ModeType == 1 )
    {
        motion_control_cmd_data.ModeType = 2;
     }
    else if(motion_control_cmd_data.ModeType == 3)
    {
        motion_control_cmd_data.ModeType = 4;
    }

    set_PC2ROVComu(1,(char*)(&motion_control_cmd_data),sizeof(motion_control_cmd_data));
    send_TcpData(1);//发送消息类型为1

}

void sendfile::sendMsgUp()
{
    qDebug() << "Button AltHOldModeOFF" ;

    if(motion_control_cmd_data.ModeType == 2 )
    {
        motion_control_cmd_data.ModeType = 1;
     }
    else if(motion_control_cmd_data.ModeType == 4)
    {
        motion_control_cmd_data.ModeType = 3;
    }


    set_PC2ROVComu(1,(char*)(&motion_control_cmd_data),sizeof(motion_control_cmd_data));
    send_TcpData(1);//发送消息类型为1

}

void sendfile::sendMsgAttHoldON()
{
    qDebug() <<  "开启定艏";

    if(motion_control_cmd_data.ModeType == 0 || motion_control_cmd_data.ModeType == 1 )
    {
        motion_control_cmd_data.ModeType = 3;
     }
    else if(motion_control_cmd_data.ModeType == 2)
    {
        motion_control_cmd_data.ModeType = 4;
    }

    set_PC2ROVComu(1,(char*)(&motion_control_cmd_data),sizeof(motion_control_cmd_data));
    send_TcpData(1);//发送消息类型为1
}

void sendfile::sendMsgAttHoldOFF()
{
    qDebug() << "关闭定艏" ;

    if(motion_control_cmd_data.ModeType == 3 )
    {
        motion_control_cmd_data.ModeType = 1;
     }
    else if(motion_control_cmd_data.ModeType == 4)
    {
        motion_control_cmd_data.ModeType = 2;
    }

    set_PC2ROVComu(1,(char*)(&motion_control_cmd_data),sizeof(motion_control_cmd_data));
    send_TcpData(1);//发送消息类型为1
}

void sendfile::sendMsgPID(PIDs_set_DATA value)
{
    qDebug() << "Seting PID" ;

    set_PC2ROVComu(2,(char*)(&value),sizeof(PIDs_set_DATA));
    send_TcpData(2);//发送消息类型为2

}

void sendfile::sendMsgBrightnessSet(int value)
{
    qDebug() << "Brightness Set" << value;
    motion_control_cmd_data.brightness = value;

    set_PC2ROVComu(1,(char*)(&motion_control_cmd_data),sizeof(motion_control_cmd_data));
    send_TcpData(1);//发送消息类型为1
}

void sendfile::sendMsgServoAngle(int8_t value)
{
    qDebug() << "servo angle set : " << value;
    motion_control_cmd_data.steering_gear_angle = value;

    set_PC2ROVComu(1,(char*)(&motion_control_cmd_data),sizeof(motion_control_cmd_data));
    send_TcpData(1);//发送消息类型为1
}

void sendfile::sendMsgTargetDepth(float value)
{
    qDebug()<<" set target depth: "<<value;
    target_control_cmd_data.target_depth = value;

    set_PC2ROVComu(3,(char*)(&target_control_cmd_data),sizeof (target_control_cmd_data));
    send_TcpData(3);

}

void sendfile::sendMsgTargetAttitude(float value)
{
    qDebug()<<" set target attitude: "<<value;
    target_control_cmd_data.target_attitude = value;
    set_PC2ROVComu(3,(char*)(&target_control_cmd_data),sizeof (target_control_cmd_data));
    send_TcpData(3);
}

//----------------------------------------------------------------
// ------------以下为封装下发指令（ PC -> ROV ）的相关函数-------------
//----------------------------------------------------------------

//包装 PC2ROVComu 的函数，传入消息类型，DATA的指针（首地址），DATA的长度
void sendfile::set_PC2ROVComu(uint8_t msg_type, char* dataPtr, int datalen)
{

    PC2ROVComu.msg_type = msg_type;                     //消息类型
    PC2ROVComu.dlen = datalen; //DATA的长度设置
    PC2ROVComu.CRC8 = get_CRC8_check_sum((unsigned char *)(&PC2ROVComu),4,0xFF); //帧头部分校验码
//    qDebug()<<"CRC8:"<<PC2ROVComu.CRC8;
    memcpy(PC2ROVComu.buff,dataPtr,datalen); //把motion_control_cmd_data中的内容给
    PC2ROVComu.CRC16 = get_CRC16_check_sum((unsigned char *)(&PC2ROVComu),datalen+5,0xFFFF);//DATA部分校验码
//    qDebug()<<"CRC16:"<<PC2ROVComu.CRC16;
}

//往 socket 中写入 PC2ROVComu 结构体。参数：消息类型
//msg_type = 1时，发送运动控制指令
//msg_type=2时，发送PID设置指令
//msg_type=3时，发送控制目标指令
void sendfile::send_TcpData(uint8_t msg_type)
{
    if(msg_type == 1)
    {
        sendTcpData.resize(sizeof(motion_control_cmd_DATA)+7);                   // sendTcpData 根据数据类型调整大小
//        qDebug()<<"sendTcpData size :"<<sendTcpData.length();
        memcpy(sendTcpData.data(),&PC2ROVComu,sizeof(motion_control_cmd_DATA)+5);//把PC2ROVComu中的所有内容发送给sendTcpData
        memcpy(sendTcpData.data()+sizeof(motion_control_cmd_DATA)+5,&PC2ROVComu.CRC16,2);//把PC2ROVComu中的所有内容发送给sendTcpData
        m_tcp->write(sendTcpData);                //发送
//        qDebug()<<"sendTcpData size :"<<sendTcpData.length();
        sendTcpData.clear();
    }
    else if(msg_type == 2)
    {
        sendTcpData.resize(sizeof(PIDs_set_DATA)+7);                   // sendTcpData 根据数据类型调整大小
        memcpy(sendTcpData.data(),&PC2ROVComu,sizeof(PIDs_set_DATA)+5);//把PC2ROVComu中的所有内容发送给sendTcpData
        memcpy(sendTcpData.data()+sizeof(PIDs_set_DATA)+5,&PC2ROVComu.CRC16,2);//把PC2ROVComu中的所有内容发送给sendTcpData
        m_tcp->write(sendTcpData);                //发送
        sendTcpData.clear();
    }
    else if(msg_type == 3)
    {
        sendTcpData.resize(sizeof(target_control_cmd_DATA)+7);                   // sendTcpData 根据数据类型调整大小
        memcpy(sendTcpData.data(),&PC2ROVComu,sizeof(target_control_cmd_DATA)+5);//把PC2ROVComu中的所有内容发送给sendTcpData
        memcpy(sendTcpData.data()+sizeof(target_control_cmd_DATA)+5,&PC2ROVComu.CRC16,2);//把PC2ROVComu中的所有内容发送给sendTcpData
        m_tcp->write(sendTcpData);                //发送
        sendTcpData.clear();
    }
}


// ---------------------------------------------------------------------
// -----------------以下为处理下位机上发（ROV->PC）的数据的相关函数-----------
// ---------------------------------------------------------------------

//void sendfile::rev_TcpData()
//{
//    recvTcpData = m_tcp->readAll();
//    qDebug()<<"接收到信息";
//    memcpy(&ROV2PCComu,recvTcpData,5);
//    if(ROV2PCComu.header == 0xA5){
//        if(verify_CRC8_check_sum((unsigned char *)(&ROV2PCComu),5))
//        {
//            switch(ROV2PCComu.msg_type)
//            {
//                case 1 : process_TcpData_type1();break;
//                case 2 : process_TcpData_type2();break;
//            }
//        }
//    }
//}

//void sendfile::process_TcpData_type1()
//{
//    memcpy(&ROV2PCComu,recvTcpData,sizeof(Robot_status_DATA)+5+2);//把所有的数据都拷贝到ROV2PCComu中
//    if(verify_CRC16_check_sum((unsigned char *)(&ROV2PCComu),sizeof(Robot_status_DATA)+5+2))
//    {
//        memcpy(&robot_status_data,recvTcpData.data()+5,sizeof(Robot_status_DATA));//把DATA部分的数据拷贝给robot_status_data
//        emit s_ROV_status(robot_status_data);
//        emit s_Yaw(robot_status_data.ROV_IMU_data.yaw);
//        emit s_altitude(robot_status_data.ROV_depth);
//    }
//}

//void sendfile::process_TcpData_type2()
//{
//    memcpy(&ROV2PCComu,recvTcpData,ROV2PCComu.dlen+5+2);//把所有的数据都拷贝到ROV2PCComu中
//    if(verify_CRC16_check_sum((unsigned char *)(&ROV2PCComu),ROV2PCComu.dlen+5+2))
//    {
//        QByteArray txt_msg;
//        memcpy(&txt_msg,recvTcpData.data()+5,ROV2PCComu.dlen);//以文本的信息输出
//        emit s_ROV_txtmsg(txt_msg);
//    }
//}


void sendfile::rev_TcpData()
{
    unsigned int totalBytes = 0;
    unsigned int recvBytes = 0;
    QByteArray block;

    if(m_tcp->bytesAvailable() == 0)
    {
//        qDebug()<<"no msg";
        return;
    }

    if(m_tcp->bytesAvailable() >= 5){
        QByteArray header = m_tcp->read(1);
        memcpy(&ROV2PCComu,header.data(),1);
        while(ROV2PCComu.header != 0xA5) // 找头
        {
//            qDebug()<<"header error";

            if(m_tcp->bytesAvailable()>0){
                header = m_tcp->read(1);
                memcpy(&ROV2PCComu,header.data(),1);
            }
            else{
                return;
            }
        }

        header.append(m_tcp->read(4));
        memcpy(&ROV2PCComu.msg_type,header.data()+1,3);

        ROV2PCComu.CRC8 = get_CRC8_check_sum((unsigned char *)(&ROV2PCComu),4,0xFF);
        uint8_t CRC8_verify = header.data()[4];

        if(ROV2PCComu.CRC8 != CRC8_verify)
        {
//            qDebug()<<"CRC8 error";
            return;
        }
        totalBytes = ROV2PCComu.dlen;
//        qDebug()<< "包头的长度"<< totalBytes;
    }
    else{
        return;
    }

    //读数据块
    while(totalBytes - recvBytes>0 && m_tcp->bytesAvailable()>0)
    {
        block.append(m_tcp->read(totalBytes - recvBytes));
        recvBytes = block.size();
    }
    if(totalBytes == recvBytes)
    {
        switch (ROV2PCComu.msg_type){
        case 1:
            {
                memcpy(&robot_status_data,block.data(),sizeof(struct Robot_status_DATA));
                memcpy(&ROV2PCComu.buff,block.data(),sizeof(struct Robot_status_DATA));

                ROV2PCComu.CRC16 = get_CRC16_check_sum((unsigned char *)(&ROV2PCComu),ROV2PCComu.dlen+5,0xFFFF);

                uint16_t CRC16_verify;
                QByteArray CRC16_verify_data = m_tcp->read(2);
                memcpy(&CRC16_verify,CRC16_verify_data.data(),2);

//                qDebug()<<"CRC16_receive:"<<ROV2PCComu.CRC16;
//                qDebug()<<"CRC16_verify:"<<CRC16_verify;

                if(ROV2PCComu.CRC16 == CRC16_verify)
                {
                    process_TcpData_type1();
                }
                else
                {
//                    qDebug()<<"CRC16 error";
                }

                //如果还有数据，继续下一个数据包
                if(m_tcp->bytesAvailable()>0)
                {
//                    qDebug()<<"开始递归调用。。。";
                    rev_TcpData();
                }
                break;
            }
        case 2:
            {
                memcpy(rov_msg_str,block.data(),ROV2PCComu.dlen);
                memcpy(&ROV2PCComu.buff,block.data(),ROV2PCComu.dlen);

                ROV2PCComu.CRC16 = get_CRC16_check_sum((unsigned char *)(&ROV2PCComu),ROV2PCComu.dlen+5,0xFFFF);
                uint16_t CRC16_verify;
                QByteArray CRC16_verify_data = m_tcp->read(2);
                memcpy(&CRC16_verify,CRC16_verify_data.data(),2);

//                qDebug()<<"CRC16_receive:"<<ROV2PCComu.CRC16;
//                qDebug()<<"CRC16_verify:"<<CRC16_verify;

                if(ROV2PCComu.CRC16 == CRC16_verify)
                {
                    process_TcpData_type2();
                }
                else
                {
//                    qDebug()<<"CRC16 error";
                }

                break;
            }
        }
    }
}

void sendfile::process_TcpData_type1()
{

    emit s_ROV_status(robot_status_data);
//    qDebug()<<"ROV_IMU_data.yaw:"<<robot_status_data.ROV_IMU_data.yaw;
//    qDebug()<<"ROV_IMU_data.ROV_depth:"<<robot_status_data.ROV_depth;

}

void sendfile::process_TcpData_type2()
{
    emit s_ROV_txtmsg(rov_msg_str);
//    qDebug()<<"接收到文本信息";
}



