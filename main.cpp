// author: zyj
// File: main.cpp

#include "mainwindow.h"
#include "startplot.h"
#include <QApplication>
#include <QDir>
#include "msg.h"

#include <QDir>
#include <QSettings>

#define MAJOR_VERSION           1
#define MINOR_VERSION           1

void FolderSettings(){
    qDebug() << "【初始化设置】";
    // 获取桌面目录
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    // 构造子目录路径
    QString dirPath = desktopPath + "/ShenHaoRov";
    // 判断子目录是否存在
    QDir dir(dirPath);
    if (!dir.exists()) {
        // 创建子目录
        if (!dir.mkdir(dirPath)) {
            qWarning() << "Failed to create directory:" << dirPath;
        } else {//目录创建成功
            qDebug() << "Directory created:" << dirPath;
        }
    } else {
        qDebug() << "Directory already exists:" << dirPath;
    }

    // 照片保存目录
    QString picturesPath = dirPath + "/Pictures";
    QDir picturesdir(picturesPath);
    if (!picturesdir.exists()) {
        if (!picturesdir.mkdir(picturesPath)) {
            qWarning() << "Failed to create directory:" << picturesPath;
        } else {
            qDebug() << "Directory created:" << picturesPath;
        }
    } else {
        qDebug() << "Directory already exists:" << picturesPath;
    }

    // 视频保存目录
    QString videosPath = dirPath + "/Videos";
    QDir videosdir(videosPath);
    if (!videosdir.exists()) {
        if (!videosdir.mkdir(videosPath)) {
            qWarning() << "Failed to create directory:" << videosPath;
        } else {
            qDebug() << "Directory created:" << videosPath;
        }
    } else {
        qDebug() << "Directory already exists:" << videosPath;
    }

    QSettings settings;
    settings.setValue("savePath/ShenHaoRov", dirPath);
    settings.setValue("savePath/Pictures", picturesPath);
    settings.setValue("savePath/Videos", videosPath);
};

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    QString version = QString::number(MAJOR_VERSION) + "." + QString::number(MINOR_VERSION);
    //设置系统使用的软件详细信息，之后所有的QSettings settings都是同一个配置文件。
    a.setOrganizationName("SH_UVBot");
    a.setOrganizationDomain("www.shenhaoinfo.com/");
    a.setApplicationName("SHGC_XUANWU_RTOS");
    a.setApplicationVersion(version);
    QSettings::setDefaultFormat(QSettings::IniFormat);    // 在运行应用程序的目录中将settinsg设置为INI格式
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, a.applicationDirPath());
    FolderSettings();//创建文件夹、并记录文件夹，用于后于视频、拍照路径

    MainWindow w;
    w.show();
    return a.exec();
}
