#pragma once
#ifndef DEFINITION_H
#define DEFINITION_H

#include "threadsafe_queue.h"
#include <QtNetwork/QUdpSocket>
#include <QQueue>

#define RECVBUFSIZE 1000    //实时数据流接收缓冲区大小
#define DATAARYSIZE 200000  //数据大数组大小
#define ANSPKTSIZE 500 //一个应答包大小
#define HMSPPIDATASIZE 300 //HMS2000 PPI模式数据量300
#define STDPPIDATASIZE 350

#define Device_STA455 3
#define Device_STD46TS 2
#define Device_HMS2000 1

typedef struct
{
    unsigned char sonardata[RECVBUFSIZE];
    unsigned int r;
}Sonar_Data;

typedef struct
{
    unsigned char sonardata_hms2000[ANSPKTSIZE];
    unsigned int data_len;
    unsigned int glb_nAngle;
}Sonar_Data_HMS2000;

typedef struct
{
    unsigned char sonardata_std46ts[ANSPKTSIZE];
    unsigned int data_len;
    unsigned int glb_nAngle;
}Sonar_Data_STD46TS;

//*******************命令包******************************
typedef struct	// Command To HMS2000 Sonar Head
{
    unsigned char ucHeader;			// 0xFE
    unsigned char ucID;				// 0x10 to 0x1f
    unsigned char ucCheckSum;		// Check sum in this version
    unsigned char ucWorkStatus;		// Bit 0 - 1 = Transmit if Bit 6 = 1 (Slave Mode)
        // Bit 1 - 1 = Send Data if Bit 6 = 1 (Slave Mode)
        // Bit 2 - 1 = Reverse, 0 = Normal operation
        // Bit 3 - 1 = Hold (or Pause) Head, 0 = Resume
        // Bit 4 - 1 Calibrate sonar head transducer (move to 0 degrees) (Slave Mode)
        // Bit 5 - 0 Normal sector, 1 one direction sector
        // Bit 6 - 0 = Master, 1 = Slave
        // Bit 7 - 0 Nomal Work, 1 Command
    unsigned char ucRange; 			// 1 to 200m
    unsigned char ucStartGain; 		// 0 - 40dB
    unsigned char ucLOGF; 			// 10,20,30,40log(r)
    unsigned char ucAbsorption; 	// dB/100m
    unsigned char ucTrainAngle; 	// 0 - 359°
    unsigned char ucSectorWidth; 	// 0 - 360°(0表示固定不转，360表示极坐标)
    unsigned char ucStepSize; 		// Bit3:0 - 步距，1 ~ 15
        // Bit 4 - 0
        // Bit 5 - 扫描宽度高位
        // Bit 6 - 扫描中心高位
        // Bit 7 - 0
    unsigned char ucPulseLength; 	// 1-100 .. 10 to 1000 usec in 10 usec increments
    unsigned char ucDataType; 		// D3:0 - D50点，以50点步进
        // Bit 4 - 0 Normal, 1 Profile
        // Bit6:5 - 00 8bits数据，一字节代表一数据点
        //			01 4bits数据，一字节代表两数据点
        //			10 16bits数据，两字节代表一数据点
        //			11 8bits数据，不去直流电平
        // Bit 7 - 0
    unsigned char ucFreq;			// 0 – 200, 200kHz to 1200khz in 5kHz increments
    unsigned char ucSoundSpeed;		// Sound speed, 0-250, c = 1375+ucSpeed
    unsigned char ucEnd;			// Termination, 0xFD
} CMD_HMS2000;

typedef struct	// Command To Sonar Head
{
    unsigned char ucHeader;			// 0xFE
    unsigned char ucID;				// 0x10 to 0x1f
    unsigned char ucReserved;		// 0
    unsigned char ucWorkStatus;		// Bit 0 - 1 = Transmit if Bit 6 = 1 (Slave Mode)
        // Bit 1 - 1 = Send Data if Bit 6 = 1 (Slave Mode)
        // Bit 2 - 1 = Reverse, 0 = Normal operation
        // Bit 3 - 1 = Hold (or Pause) Head, 0 = Resume
        // Bit 4 - 1 Calibrate sonar head transducer (move to 0 degrees) (Slave Mode)
        // Bit 5 - 0 Normal sector, 1 one direction sector
        // Bit 6 - 0 = Master, 1 = Slave
        // Bit 7 - 0 Nomal Work, 1 Command
    unsigned char ucRange; 			// 1 to 200m
    unsigned char ucStartGain; 		// 0 - 40dB
    unsigned char ucLOGF; 			// 10,20,30,40log(r)
    unsigned char ucAbsorption; 	// dB/100m
    unsigned char ucTrainAngle; 	// 与ucStepSize bit6组合，0 - 359°
    unsigned char ucSectorWidth; 	// 与ucStepSize bit5组合，0 - 360°(0表示固定不转，360表示极坐标)
    unsigned char ucStepSize; 		// Bit3:0 - 步距，1 ~ 15
        // Bit 4 - 0
        // Bit 5 - 扫描宽度高位
        // Bit 6 - 扫描中心高位
        // Bit 7 - 0
    unsigned char ucPulseLength; 	// 1-100 .. 10 to 1000 usec in 10 usec increments
    unsigned char ucDataType; 		// D3:0 - D50点，以50点步进
        // Bit 4 - 0 Normal, 1 Profile
        // Bit6:5 - 00 8bits数据，一字节代表一数据点
        //			01 4bits数据，一字节代表两数据点
        //			10 16bits数据，两字节代表一数据点
        //			11 8bits数据，压缩算法
        // Bit 7 - 0
    unsigned char ucFreq;			// 0 – 200, 200kHz to 1200khz in 5kHz increments
    unsigned char ucSoundSpeed;		// Sound speed, 0-250, c = 1375+ucSpeed
    unsigned char ucEnd;			// Termination, 0xFD
} CMD_STD46TS;

typedef struct // Command to NET Converter
{
    unsigned char ucHeader;   // 0xFF
    unsigned char ucValid;   // bit 0: 1 -> StD46TS 有效， 0 无效
        // bit 1: 1 -> HMS2000 有效， 0 无效
        // bit 2: 1 -> 高度计命令，与StD46TS命令同步
    CMD_STD46TS StD46TS;
    CMD_HMS2000 HMS2000;
} CMDFormat;

//**************************应答包帧头***************************
typedef struct	// Model HFIS Switch Echo Data
{
    unsigned char ucDevice;                 //1：HMS2000，2：StD46TS，3：高度计
    unsigned char ucLenLo;                  //有效数据长度
    unsigned char ucLenHi;                  //有效数据长度
    unsigned char ucFlag;	 //地址0x81		// 0xFE
    unsigned char ucID;		 //数据千位		// 0x1 to 0xfc
    unsigned char ucStatus;	 //数据百位		// Bit1:0 - 00 8bits数据，一字节代表一数据点
        //			01 4bits数据，一字节代表两数据点
        //			10 16bits数据，两字节代表一数据点
        //			11 保留
        // Bit 2 -  0 Normal, 1 Profile
        // Bit 3 -  0 clockwise, 1 counter-clockwise
        // Bit 4 - 	0 Normal, 1 Zero not found when angle=0
        // Bit 5 -	0
        // Bit 6 -	0 Sonar Work Status, 1 - Sonar Status
        // Bit 7 -  0

    unsigned char ucDataLo;	 //数据十位		// Echo Package length
    unsigned char ucDataHi;	 //数据个位		// len = (((ucDataHi&0x7f)<<7)|(ucDataLo&0x7f))
    unsigned char ucAngleLo; //校验  		// angle = (((ucAngleHi&0x7f)<<7)|(ucAngleLo&0x7f))
    unsigned char ucAngleHi;

    unsigned char ucEnd;			        // Termination, 0xFD
} ANSHEAD;

//************************全局变量*****************************************************
extern bool RecvRuning;

extern Sonar_Data sonardata_in;
extern Sonar_Data sonardata_out;
extern threadsafe_queue< Sonar_Data > q_Sonardata;

extern unsigned char DATAARY[DATAARYSIZE];//数据大数组 20000
extern int DATAARYHead; //数据大数组累计收到字节数
extern int DATAARYTail; //数据大数组累计取出字节数

extern Sonar_Data_HMS2000 sonardatahms2000_in; //hms2000实际数据包队列入包
extern Sonar_Data_HMS2000 sonardatahms2000_out; //hms2000实际数据包队列出包
extern threadsafe_queue < Sonar_Data_HMS2000 > q_sonardatahms2000; //hms2000实际数据包队列
extern uint q_sonardatahms2000_num;

extern Sonar_Data_STD46TS sonardatastd46ts_in; //std46ts实际数据包队列入包
extern Sonar_Data_STD46TS sonardatastd46ts_out; //std46ts实际数据包队列出包
extern threadsafe_queue < Sonar_Data_STD46TS > q_sonardatastd46ts; //std46ts实际数据包队列
extern uint q_sonardatastd46ts_num;

extern ANSHEAD anshead;
extern int RTHeight;

extern CMDFormat simucmd;



void simu_cmd();
void updateCheckSum();

#endif // DEFINITION_H
