#include "labtwo.h"
#include "ui_labtwo.h"

#include <QInputDialog>
#include <QSpinBox>
#include <QDebug>

LabTwo::LabTwo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LabTwo)
    , timer(new QTimer(this))
    , currentTime(0)
    , isRunning(false)
    , currentAlgorithm(0)
{
    ui->setupUi(this);
    setWindowTitle("实验二");
    QObject::disconnect(ui->loadDataButton,nullptr,this,nullptr);
    QObject::disconnect(ui->compareButton,nullptr,this,nullptr);
    QObject::disconnect(ui->addProcessButton,nullptr,this,nullptr);

    connect(ui->loadDataButton, &QPushButton::clicked, this, &LabTwo::on_loadDataButton_clicked);
    connect(ui->startButton, &QPushButton::clicked, this, &LabTwo::on_startButton_clicked);
    connect(ui->pauseButton, &QPushButton::clicked, this, &LabTwo::on_pauseButton_clicked);
    connect(ui->compareButton, &QPushButton::clicked, this, &LabTwo::on_compareButton_clicked);
    connect(ui->continueButton, &QPushButton::clicked, this, &LabTwo::on_continueButton_clicked);
    connect(ui->addProcessButton, &QPushButton::clicked, this, &LabTwo::on_addProcessButton_clicked);

    connect(timer, &QTimer::timeout, this, &LabTwo::schedule);
}

LabTwo::~LabTwo()
{
    delete ui;
}

void LabTwo::on_loadDataButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "打开样例数据文件", "../../", "Text Files (*.txt);;All Files (*)");
    if (!fileName.isEmpty()) {
        loadSampleData(fileName);
    }
}

void LabTwo::on_startButton_clicked()
{
    if (!isRunning && !processList.isEmpty()) {
        currentTime = 0; // 初始化当前时间
        currentAlgorithm = ui->comboBox->currentIndex();
        processList = originalList;
        initializeReadyQueue(); // 初始化就绪队列
        timer->start(1000); // 每秒调度一次
        isRunning = true;
    }
}

void LabTwo::on_continueButton_clicked()
{
    if (!isRunning && !readyQueue.isEmpty()) {
        timer->start(1000); // 每秒调度一次
        isRunning = true;
    }
}

void LabTwo::on_pauseButton_clicked()
{
    if (isRunning) {
        timer->stop();
        isRunning = false;
    }
}

void LabTwo::on_compareButton_clicked()
{
    updatePerformanceMetrics();
}

void LabTwo::on_addProcessButton_clicked()
{
    if (isRunning) {
        QMessageBox::warning(this, "警告", "进程调度已开始，无法添加新的进程。");
        return;
    }

    bool ok;
    int additionalProcessCount = QInputDialog::getInt(this, tr("设置进程数"), tr("请输入要添加的进程数:"), 1, 1, 100, 1, &ok);
    if (!ok) return;

    int currentProcessCount = processList.size();

    for (int i = 0; i < additionalProcessCount; ++i) {
        int processNumber = currentProcessCount + i + 1;
        int arrivalTime = QInputDialog::getInt(this, tr("设置进入内存时间"), tr("请输入进程 %1 的进入内存时间:").arg(processNumber), 0, 0, INT_MAX, 1, &ok);
        if (!ok) return;

        int serviceTime = QInputDialog::getInt(this, tr("设置要求服务时间"), tr("请输入进程 %1 的要求服务时间:").arg(processNumber), 1, 1, INT_MAX, 1, &ok);
        if (!ok) return;

        int jobSize = QInputDialog::getInt(this, tr("设置作业大小"), tr("请输入进程 %1 的作业大小:").arg(processNumber), 1, 1, INT_MAX, 1, &ok);
        if (!ok) return;

        int priority = QInputDialog::getInt(this, tr("设置优先级"), tr("请输入进程 %1 的优先级:").arg(processNumber), 0, 0, INT_MAX, 1, &ok);
        if (!ok) return;

        PCB process;
        process.PID = processList.size();
        process.ArrivalTime = arrivalTime;
        process.ServiceTime = serviceTime;
        process.JobSize = jobSize;
        process.Priority = priority;
        process.RemainingTime = serviceTime;
        process.State = "Ready";
        process.WaitTime = 0;
        process.FinishTime = 0;
        processList.append(process);
    }

    originalList = processList;
    displayProcessStates();
}

void LabTwo::loadSampleData(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "警告", "无法打开文件");
        return;
    }

    QTextStream in(&file);
    processList.clear(); // 清空当前的进程列表

    bool isDataValid = true; // 用于判断数据是否有效

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList data = line.split(",");
        if (data.size() == 4) {
            PCB process;
            bool ok;
            process.PID = processList.size();
            process.ArrivalTime = data[0].toInt(&ok);
            if (!ok) {
                qDebug() << "到达时间解析错误：" << data[0];
                isDataValid = false;
            }
            process.ServiceTime = data[1].toInt(&ok);
            if (!ok) {
                qDebug() << "服务时间解析错误：" << data[1];
                isDataValid = false;
            }
            process.JobSize = data[2].toInt(&ok);
            if (!ok) {
                qDebug() << "作业大小解析错误：" << data[2];
                isDataValid = false;
            }
            process.Priority = data[3].toInt(&ok);
            if (!ok) {
                qDebug() << "优先级解析错误：" << data[3];
                isDataValid = false;
            }
            process.RemainingTime = process.ServiceTime;
            process.State = "Ready";
            process.WaitTime = 0;
            process.FinishTime = 0;
            processList.append(process);
        } else {
            qDebug() << "数据格式错误：" << line;
            isDataValid = false;
            break; // 数据格式不正确，停止读取
        }
    }
    file.close();

    if (isDataValid) {
        QMessageBox::information(this, "信息", "数据成功载入");
    } else {
        QMessageBox::warning(this, "警告", "数据格式错误");
        processList.clear(); // 如果数据无效，清空进程列表
    }

    originalList = processList; // 保存原始进程列表
    displayProcessStates();
}

void LabTwo::updateProcessList(const PCB& process)
{
    for (auto &pcb : processList) {
        if (pcb.PID == process.PID) {
            pcb = process;
            break;
        }
    }
}

void LabTwo::displayProcessStates()
{
    ui->resultTableWidget->setRowCount(processList.size()); // 设置行数
    for (int row = 0; row < processList.size(); ++row) {
        const PCB &process = processList[row];
        ui->resultTableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(process.PID)));
        ui->resultTableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(process.ArrivalTime)));
        ui->resultTableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(process.ServiceTime)));
        ui->resultTableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(process.JobSize)));
        ui->resultTableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(process.Priority)));
        ui->resultTableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(process.RemainingTime)));
        ui->resultTableWidget->setItem(row, 6, new QTableWidgetItem(process.State));
        ui->resultTableWidget->setItem(row, 7, new QTableWidgetItem(QString::number(process.FinishTime)));
    }
}

void LabTwo::initializeReadyQueue()
{
    readyQueue.clear();
    for (PCB &process : processList) {
        if (process.ArrivalTime <= currentTime) {
            readyQueue.append(process);
        }
    }
}

void LabTwo::updateReadyQueue()
{
    for (int i = 0; i < processList.size(); ++i) {
        if (processList[i].ArrivalTime <= currentTime && !readyQueue.contains(processList[i]) && processList[i].State != "Finished") {
            readyQueue.append(processList[i]);
        }
    }
}

void LabTwo::roundRobin()
{
    if (readyQueue.isEmpty()) {
        return;
    }

    PCB process = readyQueue.front();
    readyQueue.removeFirst();
    process.State = "Running";

    // 执行一个时间片
    process.RemainingTime--;

    if (process.RemainingTime <= 0) {
        // 进程完成
        process.State = "Finished";
        process.FinishTime = currentTime;
    } else {
        // 进程未完成，重新放回队列
        process.State = "Ready";
        readyQueue.append(process);
    }

    // 更新当前运行的进程状态
    updateProcessList(process);

    if (!readyQueue.isEmpty()) {
        PCB nextProcess = readyQueue.front();
        nextProcess.State = "Running";
        updateProcessList(nextProcess);
    }
}

void LabTwo::firstComeFirstServe()
{
    // 如果就绪队列为空，则直接返回
    if (readyQueue.isEmpty()) {
        return;
    }

    // 对进程列表按到达时间进行排序
    std::sort(readyQueue.begin(), readyQueue.end(), [](const PCB &a, const PCB &b) {
        return a.ArrivalTime < b.ArrivalTime;
    });

    // 取出就绪队列中的第一个进程
    PCB process = readyQueue.front();
    readyQueue.removeFirst();

    // 设置当前进程状态为Running
    process.State = "Running";

    // 模拟进程运行
    process.RemainingTime--;

    if (process.RemainingTime <= 0) {
        // 进程完成
        process.State = "Finished";
        process.FinishTime = currentTime;
    } else {
        readyQueue.append(process);
    }

    updateProcessList(process); // 更新processList

    if (!readyQueue.isEmpty()) {
        readyQueue.front().State = "Running";
    }
}

void LabTwo::shortestJobFirst()
{
    if (readyQueue.isEmpty()) {
        return;
    }

    // 对就绪队列按剩余时间排序
    std::sort(readyQueue.begin(), readyQueue.end(), [](const PCB &a, const PCB &b) {
        return a.JobSize < b.JobSize;
    });

    // 取出就绪队列中的第一个进程
    PCB process = readyQueue.front();
    readyQueue.removeFirst();

    // 设置当前进程状态为Running
    process.State = "Running";

    // 将所有其他运行中的进程状态设置为Ready
    for (auto &p : readyQueue) {
        if (p.State == "Running") {
            p.State = "Ready";
        }
    }

    // 模拟进程运行一个时间单位
    process.RemainingTime--;

    if (process.RemainingTime <= 0) {
        // 进程完成
        process.State = "Finished";
        process.FinishTime = currentTime;
    } else {
        readyQueue.append(process);
    }

    updateProcessList(process); // 更新processList

    if (!readyQueue.isEmpty()) {
        PCB nextProcess = readyQueue.front();
        nextProcess.State = "Running";
        updateProcessList(nextProcess);
    }
}

void LabTwo::staticPriority()
{
    if (readyQueue.isEmpty()) {
        return;
    }

    // 对就绪队列按优先级排序
    std::sort(readyQueue.begin(), readyQueue.end(), [](const PCB &a, const PCB &b) {
        return a.Priority > b.Priority; // 优先级数值越大，优先级越高
    });

    // 取出就绪队列中的第一个进程
    PCB process = readyQueue.front();
    readyQueue.removeFirst();

    // 将所有其他运行中的进程状态设置为Ready
    for (auto &p : readyQueue) {
        if (p.State == "Running") {
            p.State = "Ready";
        }
    }

    // 设置当前进程状态为Running
    process.State = "Running";

    // 模拟进程运行一个时间单位
    process.RemainingTime--;

    if (process.RemainingTime <= 0) {
        // 进程完成
        process.State = "Finished";
        process.FinishTime = currentTime;
    } else {
        readyQueue.append(process);
    }

    updateProcessList(process); // 更新processList
}

void LabTwo::highestResponseRatioNext()
{
    if (readyQueue.isEmpty()) {
        return;
    }

    // 更新就绪队列中的等待时间
    for (auto &pcb : readyQueue) {
        if (pcb.State == "Ready") {
            pcb.WaitTime++;
        }
    }

    // 计算每个进程的响应比，并按响应比排序
    std::sort(readyQueue.begin(), readyQueue.end(), [](const PCB &a, const PCB &b) {
        double responseRatioA = (a.WaitTime + a.ServiceTime) / static_cast<double>(a.ServiceTime);
        double responseRatioB = (b.WaitTime + b.ServiceTime) / static_cast<double>(b.ServiceTime);
        return responseRatioA > responseRatioB;
    });

    // 取出就绪队列中的第一个进程
    PCB process = readyQueue.front();
    readyQueue.removeFirst();

    // 将所有其他运行中的进程状态设置为Ready
    for (auto &p : readyQueue) {
        if (p.State == "Running") {
            p.State = "Ready";
        }
    }

    // 设置当前进程状态为Running
    process.State = "Running";

    // 模拟进程运行一个时间单位
    process.RemainingTime--;

    if (process.RemainingTime <= 0) {
        // 进程完成
        process.State = "Finished";
        process.FinishTime = currentTime;
    } else {
        readyQueue.append(process);
    }

    updateProcessList(process); // 更新processList

    // 确保下一个要运行的进程状态为 "Running"
    if (!readyQueue.isEmpty()) {
        readyQueue.front().State = "Running";
    }
}

void LabTwo::calculatePerformanceMetrics(const QVector<PCB>& processes, double& avgTurnaroundTime, double& avgWaitTime)
{
    double totalTurnaroundTime = 0;
    double totalWaitTime = 0;
    int completedProcesses = 0;

    for (const PCB &process : processes) {
        if (process.State == "Finished") {
            double turnaroundTime = process.FinishTime - process.ArrivalTime;
            double waitTime = turnaroundTime - process.ServiceTime;

            totalTurnaroundTime += turnaroundTime;
            totalWaitTime += waitTime;
            completedProcesses++;
        }
    }

    if (completedProcesses == 0) {
        avgTurnaroundTime = 0;
        avgWaitTime = 0;
    } else {
        avgTurnaroundTime = totalTurnaroundTime / completedProcesses;
        avgWaitTime = totalWaitTime / completedProcesses;
    }
}

void LabTwo::updatePerformanceMetrics()
{
    QVector<double> avgTurnaroundTimes(5, 0);
    QVector<double> avgWaitTimes(5, 0);
    QStringList algorithms = {"轮转法", "先来先服务", "短作业优先", "静态优先级优先", "高响应比"};

    for (int i = 0; i < 5; ++i) {
        // 设置当前调度算法并初始化就绪队列和当前时间
        currentAlgorithm = i;
        processList = originalList;
        readyQueue.clear();
        currentTime = 0;
        isRunning = true;

        // 执行调度算法直到所有进程完成
        while (isRunning) {
            schedule();
        }

        // 计算性能指标
        double avgTurnaroundTime, avgWaitTime;
        calculatePerformanceMetrics(processList, avgTurnaroundTime, avgWaitTime);
        avgTurnaroundTimes[i] = avgTurnaroundTime;
        avgWaitTimes[i] = avgWaitTime;

        // 恢复原始进程列表
        processList = originalList;
    }

    // 显示性能比较结果
    QString result;
    for (int i = 0; i < 5; ++i) {
        result += QString("%1: 平均周转时间: %2s 平均等待时间: %3s\n")
                      .arg(algorithms[i])
                      .arg(avgTurnaroundTimes[i])
                      .arg(avgWaitTimes[i]);
    }

    QMessageBox::information(this, "性能比较", result);
}

void LabTwo::schedule()
{
    if (readyQueue.isEmpty() && !std::any_of(processList.begin(), processList.end(), [](const PCB &pcb) { return pcb.State == "Ready"; })) {
        timer->stop();
        isRunning = false;
        return;
    }

    updateReadyQueue(); // 更新就绪队列
    currentTime++; // 增加当前时间
    displayProcessStates(); // 调度前更新显示

    switch (currentAlgorithm)
    {
    case 0: roundRobin(); break;
    case 1: firstComeFirstServe(); break;
    case 2: shortestJobFirst(); break;
    case 3: staticPriority(); break;
    case 4: highestResponseRatioNext(); break;
    default: break;
    }

    displayProcessStates(); // 每次调度后更新显示
}
