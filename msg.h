#ifndef MSG_H
#define MSG_H
#pragma pack(1)     //设置结构体为1字节对齐
#define DFT_ROBOT_COMU_RT_LENGTH    249
#define MOV_CMD_ID 1;
#define SET_PID_ID 2;

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
/*Unsigned*/
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

#define SPEED_ADJUST_COMMON 0
#define SPEED_ADJUST_ACTION 1

struct msg_attitude
{
    int THR;
    int YAW;//回转自由度
    int ROL;
    int PIT;

};

class RobotComuPacket
{
public:
    unsigned int header;//帧头，在通讯协议规定下统一赋值为0XA5
    struct msg_attitude attitude;//机器人位姿
    int altitude;//机器人深度
    unsigned int end;//帧尾
};

//----------------------------------------------
//--------------- ROV->PC ---------------------

struct motors_status
{
    uint16_t thruster_speed_1;//左上推进器的速度
    uint16_t thruster_speed_2;//右上推进器的速度
    uint16_t thruster_speed_3;//左下推进器的速度
    uint16_t thruster_speed_4;//右下推进器的速度
    uint16_t thruster_speed_5;//左侧推进器的速度
    uint16_t thruster_speed_6;//右侧推进器的速度
    uint16_t crawler_motor_1;//左履带电机的速度
    uint16_t crawler_motor_2;//右履带电机的速度
};

struct IMU_data_t
{
    float yaw;				//姿态角，单位为°
    float pitch;
    float roll;
    float ax;				//加速度数据，单位为g
    float ay;
    float az;
    float gx;				//角速度数据，单位为°/s
    float gy;
    float gz;
    float mx;				//磁力计数据，单位为Gs
    float my;
    float mz;
    int32_t press;			//气压计，缺省
    float temperate;		//温度，单位为℃
    int32_t timeSample;	//时间戳，缺省
    uint8_t m_flag;			//磁状态标志，缺省
};

struct cabin_status
{
    float cabin_temperature;//舱内温度
    float cabin_humidity;//舱内湿度
    float cabin_water_level;//舱内水位
    float cpu_temperature;//CPU温度
};



struct Robot_status_DATA
{
    motors_status ROV_motors_status;//电机运动状态
    IMU_data_t ROV_IMU_data;        //IMU信息
    float ROV_depth;                //深度信息
    float ROV_height;               //高度信息
    cabin_status ROV_cabin_status;  //舱内信息
    uint8_t ROV_run_mode;          //运动模式
};

//--------------- ROV->PC ---------------------
//----------------------------------------------
//---------------绘图信息结构体--------------------
struct plot_msg
{
    uint8_t index;
    uint8_t canves_index;
    char  name[10];
    float value;
};

//--------------------------------------------------
//--------------- PC ->ROV -------------------------
struct motion_control_cmd_DATA
{
    int YAW;    //+-0-100
    int ROL;
    int PIT;
    int VX;
    int VY;
    int VZ;
    int8_t steering_gear_angle;//舵机角度，初始为0，范围：-90~90
    uint8_t brightness;//亮度范围：0~100
    uint8_t ModeType;//运行模式：0-无力，1-开环，2-定深度，3-定航，4-定深定航，5-爬壁，6-陆地爬行，7停止（闭环）
};

struct PID_set
{
    int P_set;
    int I_set;
    int D_set;
};

struct PIDs_set_DATA
{
    PID_set depth_loop_pid;						//定深环PID
    PID_set track_speed_pid;					//履带电机闭环PID
    PID_set track_turn_loop_pid;				//履带模式转向环PID
    PID_set yaw_angle_pid;              		//yaw angle PID.Yaw角度pid
    PID_set yaw_angular_velocity_pid;          	//yaw angular velocity PID.Yaw角速度pid
    PID_set pitch_angle_pid;              		//pitch angle PID.Pitch角度pid
    PID_set pitch_angular_velocity_pid;        	//pitch angular velocity PID.Pitch角速度pid
    PID_set roll_angle_pid;              		//roll angle PID.Roll角度pid
    PID_set roll_angular_velocity_pid;        	//roll angular velocity PID.Roll角速度pid
};

//消息类型3
struct target_control_cmd_DATA
{
    float target_depth;
    float target_attitude;
};

//--------------- PC ->ROV ------------------------------------
//-------------------------------------------------------------
//----封装数据（交流）包，由帧头部分+数据部分组成，有协议确定并规范------

class ROVComuPacket
{
public:
    uint8_t header;//帧头，在通讯协议规定下统一赋值为0XA5
    uint8_t  msg_type;
    uint16_t dlen;
    uint8_t  CRC8;
    uint8_t buff[DFT_ROBOT_COMU_RT_LENGTH] = { 0 };//数据部分，最大容量 249 Byte
    uint16_t CRC16;
};

//----封装数据（交流）包，由帧头部分+数据部分组成，有协议确定并规范-------
//---------------------------------------------------------------
//声纳设置界面与后端传递的结构体
struct Sonar_set
{
    int range;
    int gain;
};

//----------------------------------------------------------------
#pragma pack()      //结束结构体对齐设置
#endif // MSG_H
