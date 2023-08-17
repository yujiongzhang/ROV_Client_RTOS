#include "sonarset.h"
#include "ui_sonarset.h"

SonarSet::SonarSet(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SonarSet)
{
    ui->setupUi(this);
    qDebug()<<"创建sonarset";
    this->setAttribute(Qt::WA_DeleteOnClose,true);
    ui->sonar_range_lineEdit->setText(QString::number(1));
    ui->sonar_gain_lineEdit->setText(QString::number(36));
}

SonarSet::~SonarSet()
{
    delete ui;
    qDebug()<<"析构sonarset";
}

void SonarSet::on_sonar_default_config_PushButton_clicked()
{
    ui->sonar_range_lineEdit->setText(QString::number(1));
    ui->sonar_gain_lineEdit->setText(QString::number(36));
}

void SonarSet::on_sonar_set_PushButton_clicked()
{
    Sonar_set my_sonar_set;
    my_sonar_set.range = ui->sonar_range_lineEdit->text().toInt();
    my_sonar_set.gain = ui->sonar_gain_lineEdit->text().toInt();
    emit s_sonar_set(my_sonar_set);
}
