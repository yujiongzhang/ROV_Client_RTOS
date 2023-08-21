#include "mainwindow.h"
#include "startplot.h"
#include <QApplication>
#include "msg.h"

#include <QDir>

int main(int argc, char *argv[])
{
    QDir path; // 创建一个QDir变量
    if (!path.exists("./sonar")) {  // 使用QDir成员函数exists()来判断文件夹是否存在
        path.mkdir("./sonar");  // 使用mkdir来创建文件夹
        qDebug()<<"成功创建 sonar 文件夹";
    }
    if (!path.exists("./recode")) {  // 使用QDir成员函数exists()来判断文件夹是否存在
        path.mkdir("./recode");  // 使用mkdir来创建文件夹
        qDebug()<<"成功创建 recode 文件夹";
    }

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
