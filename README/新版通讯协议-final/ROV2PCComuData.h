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
	float yaw;					//姿态角，单位为°
	float pitch;
	float roll;
	float ax;						//加速度数据，单位为g
	float ay;
	float az;
	float gx;						//角速度数据，单位为°/s
	float gy;
	float gz;
	float mx;						//磁力计数据，单位为Gs
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
    motors_status ROV_motors_status;//电机运动状态 8*2
    IMU_data_t ROV_IMU_data;        //IMU信息  15*4+1
    float ROV_depth;                //深度信息 4
    cabin_status ROV_cabin_status;  //舱内信息 4*4
    uint8_t run_mode;               //运动模式 1
};
