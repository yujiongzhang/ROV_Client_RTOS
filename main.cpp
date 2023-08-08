#include "mainwindow.h"
#include "startplot.h"
#include <QApplication>
#include "msg.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
