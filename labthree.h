#ifndef LABTHREE_H
#define LABTHREE_H

#include <QMainWindow>
#include <QSharedPointer>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include <QPushButton>
#include <QSlider>
#include <QTextEdit>
#include <QLabel>

class Buffer
{
public:
    Buffer(int size);
    bool addProduct(const QString &product);
    bool removeProduct(const QString &product);
    QString toString() const;

private:
    QVector<QString> buffer;
    int size;
    QWaitCondition notFull;
    QWaitCondition notEmpty;
};
class Producer : public QThread
{
    Q_OBJECT
public:
    Producer(const QString &productType, int speed, Buffer *buffer, QObject *parent = nullptr);
    void run() override;
    void setSpeed(int speed);
    QString getProductType() const;

private:
    QString productType;
    volatile int speed; // 使用volatile确保速度值的变动能被各线程感知
    Buffer *buffer;
};

class Consumer : public QThread
{
    Q_OBJECT
public:
    Consumer(const QString &productType, int speed, Buffer *buffer, QObject *parent = nullptr);
    void run() override;
    void setSpeed(int speed);
    QString getProductType() const;

private:
    QString productType;
    int speed;
    Buffer *buffer;
    bool running;
};
class LabThree : public QMainWindow
{
    Q_OBJECT

public:
    LabThree(QWidget *parent = nullptr);
    ~LabThree();

private slots:
    void startSimulation();
    void stopSimulation();
    void updateBuffer();

private:
    void setupUI();

    Buffer *buffer;
    QVector<QSharedPointer<Producer>> producers;
    QVector<QSharedPointer<Consumer>> consumers;

    QPushButton *startButton;
    QPushButton *stopButton;
    QSlider *orangeProducerSpeedSlider;
    QSlider *appleProducerSpeedSlider;
    QSlider *orangeConsumerSpeedSlider;
    QSlider *appleConsumerSpeedSlider;
    QTextEdit *bufferDisplay;
    QLabel *orangeProducerLabel;
    QLabel *appleProducerLabel;
    QLabel *orangeConsumerLabel;
    QLabel *appleConsumerLabel;
    QTimer *updateTimer;
};

#endif // LABTHREE_H
