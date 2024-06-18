#ifndef LABEIGHT_H
#define LABEIGHT_H

#include <QMainWindow>
#include <QSharedMemory>
#include <QSemaphore>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>

class LabEight : public QMainWindow
{
    Q_OBJECT

public:
    LabEight(QWidget *parent = nullptr);
    ~LabEight();

private slots:
    void startChildProcess();
    void writeToSharedMemory();
    void readFromSharedMemory();

private:
    QSharedMemory sharedMemory;
    QSemaphore *semaphore;
    QPushButton *startButton;
    QPushButton *writeButton;
    QPushButton *readButton;
    QTextEdit *logTextEdit;
    QLineEdit *inputLineEdit;
};

#endif // LABEIGHT_H
