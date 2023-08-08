#include "definition.h"
#include <QDebug>
#include <QQueue>


bool RecvRuning = false;

Sonar_Data sonardata_in; //实时数据流队列入包
Sonar_Data sonardata_out; //实时数据流队列出包
threadsafe_queue< Sonar_Data > q_Sonardata; //实时数据流队列

Sonar_Data_HMS2000 sonardatahms2000_in; //hms2000实际数据包队列入包
Sonar_Data_HMS2000 sonardatahms2000_out; //hms2000实际数据包队列出包
//QQueue < Sonar_Data_HMS2000 > q_sonardatahms2000; //hms2000实际数据包队列
threadsafe_queue < Sonar_Data_HMS2000 > q_sonardatahms2000; //hms2000实际数据包队列
uint q_sonardatahms2000_num;

Sonar_Data_STD46TS sonardatastd46ts_in; //std46ts实际数据包队列入包
Sonar_Data_STD46TS sonardatastd46ts_out; //std46ts实际数据包队列出包
//QQueue < Sonar_Data_STD46TS > q_sonardatastd46ts; //std46ts实际数据包队列
threadsafe_queue < Sonar_Data_STD46TS > q_sonardatastd46ts; //std46ts实际数据包队列
uint q_sonardatastd46ts_num;

unsigned char DATAARY[DATAARYSIZE];//数据大数组 20000
int DATAARYHead = 0; //数据大数组累计收到字节数
int DATAARYTail = 0; //数据大数组累计取出字节数

ANSHEAD anshead; //处理数据时拷贝的数据头

CMDFormat simucmd; //下发命令包

int RTHeight;//处理获得的实时高度


//初始化命令包
void simu_cmd()
{
    simucmd.ucHeader = 0xFF;
    simucmd.ucValid = 0x07;
    //simucmd.StD46TS = {0xfe, 0x00, 0x00, 0x43, 100, 0, 40, 10, 0, 0x6b, 0x24, 20, 0x08, 91, 125, 0xfd};
    //    simucmd.StD46TS = {0xfe, 0x14, 0x00, 0x43, 0x01, 0x00, 0x28, 0x11, 0x00, 0x6b, 0x24, 0x0a, 0x08, 0x5b, 0x7d, 0xfd};
    //    simucmd.StD46TS = {0xfe, 0x14, 0x00, 0x43, 0x01, 0x03, 0x28, 0x11, 0x00, 0x6b, 0x24, 0x0a, 0x08, 0x5b, 0x7d, 0xfd};
    simucmd.StD46TS.ucHeader = 0xFE;
    simucmd.StD46TS.ucID = 0x14;
    simucmd.StD46TS.ucReserved = 0x00;
    simucmd.StD46TS.ucWorkStatus = 0x23;
    simucmd.StD46TS.ucRange = 0x02;
    simucmd.StD46TS.ucStartGain = 0x03;
    simucmd.StD46TS.ucLOGF = 0x28;
    simucmd.StD46TS.ucAbsorption = 0x11;
    simucmd.StD46TS.ucTrainAngle = 0x00;
    simucmd.StD46TS.ucSectorWidth = 0x6b;
    simucmd.StD46TS.ucStepSize = 0x24;
    simucmd.StD46TS.ucPulseLength = 0x0a;
    simucmd.StD46TS.ucDataType = 0x08;
    simucmd.StD46TS.ucFreq = 0x5b;
    simucmd.StD46TS.ucSoundSpeed = 0x7d;
    simucmd.StD46TS.ucEnd = 0xFD;

    //simucmd.HMS2000 = {0xfe, 128, 0, 0x03, 5, 30, 0x28, 27, 0, 90, 4, 20, 0x5|0x60, 221, 125, 0xfd};
    //simucmd.HMS2000 = {0xfe, 0x80, 0x00, 0x23, 0x01, 0x1e, 0x28, 0x11, 0x00, 0x6b, 0x24, 0x14, 0x65, 0x44, 0x7d, 0xfd};
    simucmd.HMS2000.ucHeader = 0xFE; //帧头
    simucmd.HMS2000.ucID = 0x80;
    simucmd.HMS2000.ucCheckSum = 0x00;
//    simucmd.HMS2000.ucWorkStatus = 0x23;//单向扇扫
//    simucmd.HMS2000.ucWorkStatus = 0x43;//
    simucmd.HMS2000.ucWorkStatus = 0x03;//正常扇扫
    //    simucmd.HMS2000.ucRange = 0x01; //{2,3,4,5,10,20,30,40,50,60}
//    simucmd.HMS2000.ucRange = 0x03;
    simucmd.HMS2000.ucRange = 0x01;

    //    simucmd.HMS2000.ucStartGain = 0x26;//0-40,高频默认30，低频默认20，高频38效果更好
    simucmd.HMS2000.ucStartGain = 36;

    simucmd.HMS2000.ucLOGF = 0x28;  //10,20,30,40 默认40

    simucmd.HMS2000.ucAbsorption = 0x1b;//高频27 低频17

    simucmd.HMS2000.ucTrainAngle = 0x00;//扫描中心，0 - 359°，与ucStepSize bit6组合

    simucmd.HMS2000.ucSectorWidth = 0x6b;//扫描范围 ucStepSize bit5作为高位
    //    simucmd.HMS2000.ucSectorWidth = 120;//扫描范围 ucStepSize bit5作为高位

    //    simucmd.HMS2000.ucStepSize = 0x21;//步距1
//    simucmd.HMS2000.ucStepSize = 0x24;//步距4
        simucmd.HMS2000.ucStepSize = 0x22;//步距2
    //    simucmd.HMS2000.ucStepSize = 0x01;//步距1
    //    simucmd.HMS2000.ucStepSize = 0x04;

    simucmd.HMS2000.ucPulseLength = 0x14; //20  1-100代表10-1000微秒

    simucmd.HMS2000.ucDataType = 0x65; //8bits数据，压缩算法
    simucmd.HMS2000.ucFreq = 0xdd; //高频0xdd 低频0x44

    simucmd.HMS2000.ucSoundSpeed = 0x7d; //1375+125  0-250
    simucmd.HMS2000.ucEnd = 0xfd; //帧尾

    unsigned char sum = 0;
    unsigned char * p = (unsigned char *)&simucmd.HMS2000;
    for(unsigned int i = 0; i<sizeof(CMD_HMS2000); i++)
    {
        sum += *p++;
    }
    simucmd.HMS2000.ucCheckSum = -sum;
    //    验证sum计算结果
    //    qDebug()<<sum;
    //    sum = 0;
    //    p = (unsigned char *)&simucmd.HMS2000;
    //    for(unsigned int i = 0; i<sizeof(CMD_HMS2000); i++)
    //    {
    //        sum += *p++;
    //    }
    //    qDebug()<<sum;
}

void updateCheckSum()
{
    simucmd.HMS2000.ucCheckSum = 0x00;
    unsigned char sum = 0;
    unsigned char * p = (unsigned char *)&simucmd.HMS2000;
    for(unsigned int i = 0; i<sizeof(CMD_HMS2000); i++)
    {
        sum += *p++;
    }
    simucmd.HMS2000.ucCheckSum = -sum;
}




