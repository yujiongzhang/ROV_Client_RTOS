#include "RtspCameraFun.h"

RtspCameraFun::RtspCameraFun()
{

}

bool isCameraPingable(const QString &IP)
{

#ifdef Q_OS_LINUX
    // Linux
    // 创建一个Ping进程对象
    QProcess pingProcess;
    // 执行Ping命令
    pingProcess.start("ping", QStringList() << "-c" << "1" << IP);
    // 等待Ping进程完成
    pingProcess.waitForFinished();
    // 获取Ping命令的输出结果
    QString output = pingProcess.readAll();
    // 检查Ping命令的输出结果是否包含 "TTL"，表示Ping通
    if (output.contains("ttl")) {
        return true;
    } else {
        return false;
    }
#elif defined(Q_OS_WIN)
    // Windows
    // 创建一个Ping进程对象
    QProcess pingProcess;
    // 执行Ping命令
    pingProcess.start("ping", QStringList() << "-n" << "1" << IP);
    // 等待Ping进程完成
    pingProcess.waitForFinished();
    // 获取Ping命令的输出结果
    QString output = pingProcess.readAll();
    // 检查Ping命令的输出结果是否包含 "TTL"，表示Ping通
    if (output.contains("TTL")) {
        return true;
    } else {
        return false;
    }
#else
    // Code for other platforms
#endif

}
