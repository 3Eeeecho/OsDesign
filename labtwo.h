#ifndef LABTWO_H
#define LABTWO_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QMessageBox>
#include <QFileDialog>
#include <QLineEdit>

namespace Ui {
class LabTwo;
}

class LabTwo : public QWidget
{
    Q_OBJECT

public:
    explicit LabTwo(QWidget *parent = nullptr);
    ~LabTwo();

private slots:
    void on_loadDataButton_clicked();
    void on_startButton_clicked();
    void on_continueButton_clicked();
    void on_pauseButton_clicked();
    void on_compareButton_clicked();
    void schedule();

    void on_addProcessButton_clicked();

private:
    Ui::LabTwo *ui;
    QTimer *timer;
    int currentTime;
    bool isRunning;
    int currentAlgorithm;

    struct PCB {
        int PID;
        int ArrivalTime;
        int ServiceTime;
        int JobSize;
        int Priority;
        int RemainingTime;
        QString State;
        int WaitTime;
        int FinishTime;

        bool operator==(const PCB& other) const {
            return PID == other.PID;
        }
    };

    QVector<PCB> processList;
    QVector<PCB> originalList;
    QVector<PCB> readyQueue;

    void loadSampleData(const QString &fileName);
    void displayProcessStates();
    void initializeReadyQueue();
    void updateReadyQueue();
    void updateProcessList(const PCB& process);
    void roundRobin();
    void firstComeFirstServe();
    void shortestJobFirst();
    void staticPriority();
    void highestResponseRatioNext();
    void calculatePerformanceMetrics(const QVector<PCB>& processes, double& avgTurnaroundTime, double& avgWaitTime);
    void updatePerformanceMetrics();
};

#endif // LABTWO_H
