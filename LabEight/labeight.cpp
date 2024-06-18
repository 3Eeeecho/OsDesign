#include "labeight.h"
#include <QProcess>
#include <QDateTime>
#include <QDebug>
#include <QInputDialog>
#include <QCoreApplication>
#include <QLabel>

LabEight::LabEight(QWidget *parent)
    : QMainWindow(parent), sharedMemory("SharedMemoryExample"), semaphore(new QSemaphore(1))
{
    // UI Setup
    startButton = new QPushButton("生成子进程", this);
    writeButton = new QPushButton("写入共享内存", this);
    readButton = new QPushButton("读取共享内存", this);
    logTextEdit = new QTextEdit(this);
    logTextEdit->setReadOnly(true);
    inputLineEdit = new QLineEdit(this); // 初始化 QLineEdit

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(startButton);
    layout->addWidget(new QLabel("输入消息:", this)); // 添加标签
    layout->addWidget(inputLineEdit); // 添加 QLineEdit 到布局
    layout->addWidget(writeButton);
    layout->addWidget(readButton);
    layout->addWidget(logTextEdit);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    connect(startButton, &QPushButton::clicked, this, &LabEight::startChildProcess);
    connect(writeButton, &QPushButton::clicked, this, &LabEight::writeToSharedMemory);
    connect(readButton, &QPushButton::clicked, this, &LabEight::readFromSharedMemory);

    setWindowTitle("主进程");

    if (!sharedMemory.create(256)) {
        logTextEdit->append("无法创建共享内存: " + sharedMemory.errorString());
    } else {
        logTextEdit->append("成功创建共享内存.");
    }
}

LabEight::~LabEight()
{
    delete semaphore;
}

void LabEight::startChildProcess()
{
    QString program = "D:/QtFiles/OsDesign/ChildProcess/build/Desktop_Qt_6_7_1_MSVC2019_64bit-Debug/debug/ChildProcess.exe";
    if (!QProcess::startDetached(program)) {
        logTextEdit->append("无法生成子进程.");
    } else {
        logTextEdit->append("子进程启动于 " + QDateTime::currentDateTime().toString());
    }
}

void LabEight::writeToSharedMemory()
{
    semaphore->acquire(); // 获取信号量

    if (sharedMemory.isAttached() || sharedMemory.attach()) {
        sharedMemory.lock();
        char *to = (char*)sharedMemory.data();
        QString userMessage = inputLineEdit->text(); // 获取用户输入的内容
        QByteArray byteArray = userMessage.toLatin1(); // 转换为字节数组
        const char *from = byteArray.data();
        memcpy(to, from, qMin(sharedMemory.size(), 256));
        sharedMemory.unlock();
        logTextEdit->append("写入共享内存于 " + QDateTime::currentDateTime().toString() + ": " + userMessage);
    } else {
        logTextEdit->append("无法附加到共享内存: " + sharedMemory.errorString());
    }

    semaphore->release(); // 释放信号量
}

void LabEight::readFromSharedMemory()
{
    semaphore->acquire(); // 获取信号量

    if (sharedMemory.isAttached() || sharedMemory.attach()) {
        sharedMemory.lock();
        char *from = (char*)sharedMemory.constData();
        QString message = QString::fromLatin1(from);
        sharedMemory.unlock();
        logTextEdit->append("读取共享内存: " + message + " 于 " + QDateTime::currentDateTime().toString());
    } else {
        logTextEdit->append("无法附加到共享内存: " + sharedMemory.errorString());
    }

    semaphore->release(); // 释放信号量
}
