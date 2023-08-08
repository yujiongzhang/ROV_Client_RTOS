#include<iostream>
#include"ROV2PCComuData.h"
#include"PC2ROVComuData.h"
using namespace std;

int main(){
    
    Robot_status_DATA a;
    motion_control_cmd_DATA mc;
    PIDs_set_DATA pidsets;

    cout<<" int8_t size : "<<sizeof(int8_t)<<endl;

    cout<<"Robot_status_DATA size : "<<sizeof(a)<<endl;
    cout<<"motion_control_cmd_DATA size : "<<sizeof(mc)<<endl;
    cout<<"PIDs_set_DATA size : "<<sizeof(pidsets)<<endl;

    system("pause");
    return 0;
}