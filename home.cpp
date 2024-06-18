#include "home.h"
#include "ui_home.h"

Home::Home(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Home)
    ,process(new QProcess(this))
{
    ui->setupUi(this);

    connect(ui->labOneButton,&QPushButton::clicked,this,&Home::on_labOneButton_clicked);
    connect(ui->labTwoButton,&QPushButton::clicked,this,&Home::on_labTwoButton_clicked);
    connect(ui->labThreeButton,&QPushButton::clicked,this,&Home::on_labThreeButton_clicked);
    connect(ui->labFourButton,&QPushButton::clicked,this,&Home::on_labFourButton_clicked);
    connect(ui->labFiveButton,&QPushButton::clicked,this,&Home::on_labFiveButton_clicked);
    connect(ui->labSixButton,&QPushButton::clicked,this,&Home::on_labSixButton_clicked);
    connect(ui->labSevenButton,&QPushButton::clicked,this,&Home::on_labSevenButton_clicked);
    connect(ui->labEightButton,&QPushButton::clicked,this,&Home::on_labEightButton_clicked);
}

Home::~Home()
{
    delete ui;
}

void Home::on_labOneButton_clicked()
{
    l1.show();
}


void Home::on_labTwoButton_clicked()
{
    l2.show();
}


void Home::on_labThreeButton_clicked()
{
    l3.show();
}


void Home::on_labFourButton_clicked()
{
    l4.show();
}


void Home::on_labFiveButton_clicked()
{
    l5.show();
}


void Home::on_labSixButton_clicked()
{
    l6.show();
}


void Home::on_labSevenButton_clicked()
{
    l7.show();
}


void Home::on_labEightButton_clicked()
{
    // 确保 LabEight.exe 的路径正确
    QString program = "D:/QtFiles/OsDesign/LabEight/build/Desktop_Qt_6_7_1_MSVC2019_64bit-Debug/debug/LabEight.exe";
    process->start(program);

    // 检查进程是否启动成功
    if (!process->waitForStarted()) {
        qDebug() << "Failed to start process:" << process->errorString();
    }
}

