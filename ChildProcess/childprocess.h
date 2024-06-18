#ifndef CHILDPROCESS_H
#define CHILDPROCESS_H

#include <QMainWindow>
#include <QSharedMemory>
#include <QSemaphore>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextEdit>

class ChildProcess : public QMainWindow
{
    Q_OBJECT

public:
    ChildProcess(QWidget *parent = nullptr);
    void start();

private slots:
    void readFromSharedMemory();
    void writeToSharedMemory();

private:
    QSharedMemory sharedMemory;
    QSemaphore *semaphore;
    QTextEdit *logTextEdit;
    QLineEdit *inputLineEdit;
    QPushButton *writeButton;
    QPushButton *readButton;
};

#endif // CHILDPROCESS_H
