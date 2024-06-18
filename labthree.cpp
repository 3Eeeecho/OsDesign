#include "labthree.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QDebug>

QMutex speedMutex;
QMutex mutex;

//Buffer
Buffer::Buffer(int size) : size(size)
{
    buffer.reserve(size);
}

bool Buffer::addProduct(const QString &product)
{
    QMutexLocker locker(&mutex);
    while (buffer.size() >= size) {
        notFull.wait(&mutex);
    }
    buffer.push_back(product);
    notEmpty.wakeAll();
    return true;
}

bool Buffer::removeProduct(const QString &product)
{
    QMutexLocker locker(&mutex);
    while (buffer.isEmpty()) {
        notEmpty.wait(&mutex);
    }
    for (int i = 0; i < buffer.size(); ++i) {
        if (buffer[i] == product) {
            buffer.remove(i);
            notFull.wakeAll();
            return true;
        }
    }
    return false;
}

QString Buffer::toString() const
{
    QMutexLocker locker(&mutex);
    QString bufferStr;
    for (const auto &item : buffer) {
        bufferStr += item + " ";
    }
    return bufferStr;
}


//Producer
Producer::Producer(const QString &productType, int speed, Buffer *buffer, QObject *parent)
    : QThread(parent), productType(productType), speed(speed), buffer(buffer)
{
}

void Producer::run()
{
    while (true) {
        {
            QMutexLocker locker(&speedMutex);
            msleep(speed);
        }
        buffer->addProduct(productType);
    }
}

void Producer::setSpeed(int speed)
{
    QMutexLocker locker(&speedMutex);
    this->speed = speed;
}

QString Producer::getProductType() const
{
    return productType;
}


//Consumer
Consumer::Consumer(const QString &productType, int speed, Buffer *buffer, QObject *parent)
    : QThread(parent), productType(productType), speed(speed), buffer(buffer)
{
}

void Consumer::run()
{
    while (true) {
        {
            QMutexLocker locker(&speedMutex);
            msleep(speed);
        }
        buffer->removeProduct(productType);
    }
}

void Consumer::setSpeed(int speed)
{
    QMutexLocker locker(&speedMutex);
    this->speed = speed;
}

QString Consumer::getProductType() const
{
    return productType;
}

LabThree::LabThree(QWidget *parent)
    : QMainWindow(parent), buffer(new Buffer(20))
{
    setupUI();

    for (int i = 0; i < 20; ++i) {
        producers.append(QSharedPointer<Producer>::create("Orange", 1000, buffer, this));
        producers.append(QSharedPointer<Producer>::create("Apple", 1000, buffer, this));
        consumers.append(QSharedPointer<Consumer>::create("Orange", 1000, buffer, this));
        consumers.append(QSharedPointer<Consumer>::create("Apple", 1000, buffer, this));
    }

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &LabThree::updateBuffer);
}

LabThree::~LabThree()
{
    stopSimulation();
    delete buffer;
}

void LabThree::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    startButton = new QPushButton("开始模拟", this);
    stopButton = new QPushButton("停止模拟", this);
    orangeProducerSpeedSlider = new QSlider(Qt::Horizontal, this);
    appleProducerSpeedSlider = new QSlider(Qt::Horizontal, this);
    orangeConsumerSpeedSlider = new QSlider(Qt::Horizontal, this);
    appleConsumerSpeedSlider = new QSlider(Qt::Horizontal, this);
    bufferDisplay = new QTextEdit(this);
    bufferDisplay->setReadOnly(true);

    orangeProducerLabel = new QLabel("橘子生成速度:", this);
    appleProducerLabel = new QLabel("苹果生成速度:", this);
    orangeConsumerLabel = new QLabel("橘子消费速度:", this);
    appleConsumerLabel = new QLabel("苹果消费速度:", this);

    orangeProducerSpeedSlider->setRange(100, 2000);
    appleProducerSpeedSlider->setRange(100, 2000);
    orangeConsumerSpeedSlider->setRange(100, 2000);
    appleConsumerSpeedSlider->setRange(100, 2000);

    connect(startButton, &QPushButton::clicked, this, &LabThree::startSimulation);
    connect(stopButton, &QPushButton::clicked, this, &LabThree::stopSimulation);

    connect(orangeProducerSpeedSlider, &QSlider::valueChanged, [this](int value) {
        for (const auto &producer : producers) {
            if (producer->getProductType() == "Orange") {
                producer->setSpeed(value);
            }
        }
    });
    connect(appleProducerSpeedSlider, &QSlider::valueChanged, [this](int value) {
        for (const auto &producer : producers) {
            if (producer->getProductType() == "Apple") {
                producer->setSpeed(value);
            }
        }
    });
    connect(orangeConsumerSpeedSlider, &QSlider::valueChanged, [this](int value) {
        for (const auto &consumer : consumers) {
            if (consumer->getProductType() == "Orange") {
                consumer->setSpeed(value);
            }
        }
    });
    connect(appleConsumerSpeedSlider, &QSlider::valueChanged, [this](int value) {
        for (const auto &consumer : consumers) {
            if (consumer->getProductType() == "Apple") {
                consumer->setSpeed(value);
            }
        }
    });


    mainLayout->addWidget(startButton);
    mainLayout->addWidget(stopButton);
    mainLayout->addWidget(orangeProducerLabel);
    mainLayout->addWidget(orangeProducerSpeedSlider);
    mainLayout->addWidget(appleProducerLabel);
    mainLayout->addWidget(appleProducerSpeedSlider);
    mainLayout->addWidget(orangeConsumerLabel);
    mainLayout->addWidget(orangeConsumerSpeedSlider);
    mainLayout->addWidget(appleConsumerLabel);
    mainLayout->addWidget(appleConsumerSpeedSlider);
    mainLayout->addWidget(new QLabel("Buffer:", this));
    mainLayout->addWidget(bufferDisplay);

    setCentralWidget(centralWidget);
    setWindowTitle("实验三");
}

void LabThree::startSimulation()
{
    for (const auto &producer : producers) {
        producer->start();
    }
    for (const auto &consumer : consumers) {
        consumer->start();
    }
    updateTimer->start(1000);
}

void LabThree::stopSimulation()
{
    for (const auto &producer : producers) {
        producer->terminate();
    }
    for (const auto &consumer : consumers) {
        consumer->terminate();
    }
    updateTimer->stop();
}

void LabThree::updateBuffer()
{
    bufferDisplay->setText(buffer->toString());
}
