#include "childprocess.h"
#include <QDebug>
#include <QDateTime>
#include <QVBoxLayout>
#include <QCoreApplication>
#include <QWidget>
#include <QLabel>

ChildProcess::ChildProcess(QWidget *parent)
    : QMainWindow(parent), sharedMemory("SharedMemoryExample"), semaphore(new QSemaphore(1))
{
    logTextEdit = new QTextEdit(this);
    logTextEdit->setReadOnly(true);
    inputLineEdit = new QLineEdit(this);
    writeButton = new QPushButton("写入共享内存", this);
    readButton = new QPushButton("读取共享内存", this);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(new QLabel("输入消息:", this));
    layout->addWidget(inputLineEdit);
    layout->addWidget(writeButton);
    layout->addWidget(readButton);
    layout->addWidget(logTextEdit);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    connect(writeButton, &QPushButton::clicked, this, &ChildProcess::writeToSharedMemory);
    connect(readButton, &QPushButton::clicked, this, &ChildProcess::readFromSharedMemory);

    setWindowTitle("子进程");
    resize(400, 300);
}

void ChildProcess::start()
{
    show(); // 显示子进程窗口
}

void ChildProcess::writeToSharedMemory()
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

void ChildProcess::readFromSharedMemory()
{
    semaphore->acquire(); // 获取信号量

    if (!sharedMemory.isAttached()) {
        if (!sharedMemory.attach()) {
            logTextEdit->append("无法附加到共享内存: " + sharedMemory.errorString());
            semaphore->release(); // 释放信号量
            return;
        }
    }

    sharedMemory.lock();
    char *from = (char*)sharedMemory.constData();
    QString message = QString::fromLatin1(from);
    sharedMemory.unlock();
    logTextEdit->append("读取共享内存: " + message + " 于 " + QDateTime::currentDateTime().toString());
    sharedMemory.detach();

    semaphore->release(); // 释放信号量
}
