#pragma pack(1)     //设置结构体为1字节对齐

struct motion_control_cmd_DATA
{
    int YAW;//0-100
    int ROL;
    int PIT;
    int VX;
    int VY;
    int VZ;
	  int8_t steering_gear_angle;//舵机角度，初始为0，范围：-90~90
	  int8_t brightness;//亮度范围：0~100
    int8_t ModeType;
};

  /*
    ModeType 0~7 分别对应：
    --------------------------------------------------------------------
    ROV_ZERO_FORCE,         //直接发送速度为0，ROV无力, 跟没上电那样
    ROV_OPEN                //开环模式
    ROV_ONLY_ALTHOLD,   	//只开启定深模式
    ROV_ONLY_ATTHOLD,   	//只开启定首模式
    ROV_NORMAL,  			//ROV正常浮游模式，开启定深和稳姿
    ROV_CRAWLING,           //陆地爬行模式--电机不运动
    ROV_STICK_WALL,         //贴壁模式，保持推进器贴紧利用履带
    ROV_NO_MOVE,            //停止模式，此时机器人稳定当前状态，进行作业（暂不开发）
    ---------------------------------------------------------------------
  */


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


#pragma pack()      //结束结构体对齐设置