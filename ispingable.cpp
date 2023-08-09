#include "ispingable.h"

bool isPingable(const char* ipAddress) {
    // 构建ping命令
    std::string command = "ping -n 1 ";
    command += ipAddress;

    // 执行ping命令
    if (system(command.c_str()) == 0) {
        return true;  // ping成功
    } else {
        std::cout<< "can't ping tong"<< ipAddress << std::endl;
        return false; // ping失败
    }
}
