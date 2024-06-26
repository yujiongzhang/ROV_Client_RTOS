// author: zyj
// File: startplot.cpp

#include "startplot.h"
#include "ui_startplot.h"

startPlot::startPlot(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::startPlot)
{
    ui->setupUi(this);

    setupRealtimeDataDemo1(ui->customPlot);//画图窗口1设置
    setupRealtimeDataDemo2(ui->customPlot2);//画图窗口2设置

    qDebug() << "startplot线程地址: " << QThread::currentThread();
    qDebug() << "创建 startPlot 类" ;

    // 设置一个重复调用 MainWindow::realtimeDataSlot 的计时器
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(timeDataSlot()));
    dataTimer.start(0); // Interval 0 means to refresh as fast as possible

    this->setAttribute(Qt::WA_DeleteOnClose,true);
}

startPlot::~startPlot()
{
    delete ui;
    qDebug() << "startPlot 类被释放" ;
}

void startPlot::setupRealtimeDataDemo1(QCustomPlot *customPlot)
{
    customPlot->addGraph(); // 添加图形1
    customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));//设置颜色
    customPlot->addGraph(); // 添加图形2
    customPlot->graph(1)->setPen(QPen(QColor(255, 40, 110)));//设置颜色
    customPlot->addGraph(); // 添加图形2
    customPlot->graph(2)->setPen(QPen(QColor(110, 255, 40)));//设置颜色

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime); //设置一个 QCPAxisTickerTime 类的共享指针
    timeTicker->setTimeFormat("%h:%m:%s");
    customPlot->xAxis->setTicker(timeTicker);
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->yAxis->setRange(-180, 360);

    // 使左轴和下轴将其范围转移到右轴和上轴
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
}

void startPlot::setupRealtimeDataDemo2(QCustomPlot *customPlot)
{
    customPlot->addGraph(); // 添加图形1
    customPlot->graph(0)->setPen(QPen(QColor(40, 255, 10)));//设置颜色

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime); //设置一个 QCPAxisTickerTime 类的共享指针
    timeTicker->setTimeFormat("%h:%m:%s");
    customPlot->xAxis->setTicker(timeTicker);
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->yAxis->setRange(-3, 3);

    // 使左轴和下轴将其范围转移到右轴和上轴
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
}

void startPlot::addDataSlot1(double _pitch, double _roll, double _yaw)
{
    ui->customPlot->graph(0)->addData(this->t, _pitch);

    ui->customPlot->graph(0)->rescaleValueAxis();//重新缩放值（垂直）轴以适合当前数据

    ui->customPlot->xAxis->setRange(this->t, 15, Qt::AlignRight);// 使键轴范围随数据滚动（恒定范围大小为15）：

    ui->customPlot->replot();
}

void startPlot::addDataSlot2(double mun)
{
    ui->customPlot2->graph(0)->addData(this->t, mun);
    ui->customPlot2->graph(0)->rescaleValueAxis();//重新缩放值（垂直）轴以适合当前数据
    ui->customPlot2->xAxis->setRange(this->t, 15, Qt::AlignRight);// 使键轴范围随数据滚动（恒定范围大小为8）：
    ui->customPlot2->replot();
}

void startPlot::updateSlot(Robot_status_DATA f_robot_status_data)
{
    addDataSlot1(f_robot_status_data.ROV_IMU_data.pitch,f_robot_status_data.ROV_IMU_data.roll,f_robot_status_data.ROV_IMU_data.yaw);
    addDataSlot2(f_robot_status_data.ROV_depth);
}

//通过时间来创建一个虚拟的模拟下位机上传
void startPlot::timeDataSlot()
{
    static QTime timeStart2 = QTime::currentTime();
    t = timeStart2.msecsTo(QTime::currentTime())/1000.0; // 项目开始后经过的时间,单位 s
}
