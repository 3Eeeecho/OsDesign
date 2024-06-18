#include "labone.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QInputDialog>

LabOne::LabOne(QWidget *parent)
    : QMainWindow(parent), resourceCount(0), processCount(0)
{
    setupUI();
    bankersAlgorithm = nullptr;

    // 设置窗口初始大小
    resize(600, 800);

    // 设置大小策略
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setSizePolicy(sizePolicy);
}

LabOne::~LabOne()
{
    delete bankersAlgorithm;
}

void LabOne::setupUI()
{
    setWindowTitle("实验一");
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    resourceCountSpinBox = new QSpinBox();
    resourceCountSpinBox->setMinimum(1);
    processCountSpinBox = new QSpinBox();
    processCountSpinBox->setMinimum(1);
    inputLayout->addWidget(new QLabel("资源种类数量:"));
    inputLayout->addWidget(resourceCountSpinBox);
    inputLayout->addWidget(new QLabel("进程数量:"));
    inputLayout->addWidget(processCountSpinBox);
    mainLayout->addLayout(inputLayout);

    QPushButton *addProcessButton = new QPushButton("添加进程");
    QPushButton *requestResourceButton = new QPushButton("请求资源");
    QPushButton *loadSampleDataButton = new QPushButton("加载样本数据");
    checkSafetyButton = new QPushButton("检查安全性");
     setInitialResourcesButton = new QPushButton("设置可用资源");

    // 设置按钮固定宽度
    addProcessButton->setFixedWidth(120);
    requestResourceButton->setFixedWidth(120);
    loadSampleDataButton->setFixedWidth(120);
    checkSafetyButton->setFixedWidth(120);
    setInitialResourcesButton->setFixedWidth(120);

    // 创建一个水平布局来居中按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();  // 添加伸展以使按钮居中
    buttonLayout->addWidget(setInitialResourcesButton); // 新增按钮
    buttonLayout->addWidget(addProcessButton);
    buttonLayout->addWidget(requestResourceButton);
    buttonLayout->addWidget(loadSampleDataButton);
    buttonLayout->addWidget(checkSafetyButton);
    buttonLayout->addStretch();  // 添加伸展以使按钮居中

    mainLayout->addLayout(buttonLayout);  // 将按钮布局添加到主布局中

    allocationTable = new QTableWidget();
    maxTable = new QTableWidget();
    needTable = new QTableWidget();
    availableTable = new QTableWidget();

    mainLayout->addWidget(new QLabel("资源分配:"));
    mainLayout->addWidget(allocationTable);
    mainLayout->addWidget(new QLabel("最大需求:"));
    mainLayout->addWidget(maxTable);
    mainLayout->addWidget(new QLabel("目前需求:"));
    mainLayout->addWidget(needTable);
    mainLayout->addWidget(new QLabel("可用资源:"));
    mainLayout->addWidget(availableTable);

    setCentralWidget(centralWidget);

    connect(addProcessButton, &QPushButton::clicked, this, &LabOne::onAddProcess);
    connect(requestResourceButton, &QPushButton::clicked, this, &LabOne::onRequestResource);
    connect(loadSampleDataButton, &QPushButton::clicked, this, &LabOne::onLoadSampleData);
    connect(checkSafetyButton, &QPushButton::clicked, this, &LabOne::onCheckSafety);
    connect(setInitialResourcesButton, &QPushButton::clicked, this, &LabOne::onSetInitialResources);
}

void LabOne::onAddProcess()
{
    // 检查资源和进程数量是否已设置
    resourceCount = resourceCountSpinBox->value();
    if (resourceCount <= 0) {
        QMessageBox::warning(this, tr("错误"), tr("资源种类数量必须大于0"));
        return;
    }

    int processID = allocation.size() + 1;  // 显示从1开始计数

    QVector<int> allocationRow(resourceCount, 0);
    QVector<int> maxRow(resourceCount, 0);
    QVector<int> needRow(resourceCount, 0);

    for (int i = 0; i < resourceCount; ++i) {
        maxRow[i] = QInputDialog::getInt(this, tr("P%1分配最大需求").arg(processID), tr("%1号资源:").arg(i), 0, 0, INT_MAX);
        needRow[i] = maxRow[i];
    }

    allocation.append(allocationRow);
    max.append(maxRow);
    need.append(needRow);

    if (bankersAlgorithm) {
        delete bankersAlgorithm;
    }
    bankersAlgorithm = new BankersAlgorithm(resourceCount, allocation.size());
    bankersAlgorithm->setAllocation(allocation);
    bankersAlgorithm->setMax(max);
    bankersAlgorithm->setNeed(need);
    bankersAlgorithm->setAvailable(available);

    updateTable();
}

void LabOne::onRequestResource()
{
    bool ok;
    int processID = QInputDialog::getInt(this, tr("请求资源"), tr("进程号:"), 1, 1, allocation.size(), 1, &ok);
    if (!ok) return;

    QVector<int> request(resourceCount, 0);
    for (int i = 0; i < resourceCount; ++i) {
        request[i] = QInputDialog::getInt(this, tr("P%1请求资源").arg(processID), tr("%1号资源:").arg(i), 0, 0, INT_MAX);
    }

    if (bankersAlgorithm->requestResource(processID - 1, request)) {
        QMessageBox::information(this, tr("请求批准"), tr("资源请求已批准。"));
    } else {
        QMessageBox::warning(this, tr("请求被拒绝"), tr("资源请求由于安全检查失败而被拒绝。"));
    }

    allocation = bankersAlgorithm->getAllocation();
    need = bankersAlgorithm->getNeed();
    available = bankersAlgorithm->getAvailable();
    updateTable();
}

void LabOne::onLoadSampleData()
{
    QString fileName = QFileDialog::getOpenFileName(this, "打开样本数据", "../../", "Text Files (*.txt)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件");
        return;
    }

    QTextStream in(&file);
    in >> resourceCount >> processCount;
    resourceCountSpinBox->setValue(resourceCount);
    processCountSpinBox->setValue(processCount);

    allocation.resize(processCount, QVector<int>(resourceCount));
    max.resize(processCount, QVector<int>(resourceCount));
    need.resize(processCount, QVector<int>(resourceCount));
    available.resize(resourceCount);

    for (int i = 0; i < processCount; ++i) {
        for (int j = 0; j < resourceCount; ++j) {
            in >> allocation[i][j];
        }
    }

    for (int i = 0; i < processCount; ++i) {
        for (int j = 0; j < resourceCount; ++j) {
            in >> max[i][j];
            need[i][j] = max[i][j] - allocation[i][j];
        }
    }

    for (int i = 0; i < resourceCount; ++i) {
        in >> available[i];
    }

    file.close();

    if (bankersAlgorithm) {
        delete bankersAlgorithm;
    }
    bankersAlgorithm = new BankersAlgorithm(resourceCount, processCount);
    bankersAlgorithm->setAllocation(allocation);
    bankersAlgorithm->setMax(max);
    bankersAlgorithm->setNeed(need);
    bankersAlgorithm->setAvailable(available);

    updateTable();
}

void LabOne::onCheckSafety()
{
    if (checkSafety()) {
        QMessageBox::information(this, tr("系统安全"), tr("系统通过安全性检测."));

        QVector<int> safetySequence = bankersAlgorithm->getSafetySequence();
        QString sequenceStr = "安全序列: ";
        for (int process : safetySequence) {
            sequenceStr += "P" + QString::number(process + 1) + " ";
        }
        QMessageBox::information(this, tr("安全序列"), sequenceStr);
    } else {
        QMessageBox::warning(this, tr("系统不安全"), tr("系统未通过安全性检测."));
    }
}

void LabOne::onSetInitialResources()
{
    resourceCount = resourceCountSpinBox->value();
    if (resourceCount <= 0) {
        QMessageBox::warning(this, tr("错误"), tr("资源种类数量必须大于0"));
        return;
    }

    available.resize(resourceCount);
    for (int i = 0; i < resourceCount; ++i) {
        available[i] = QInputDialog::getInt(this, tr("可用资源"), tr("资源 %1:").arg(i), 0, 0, INT_MAX);
    }

    if (bankersAlgorithm) {
        bankersAlgorithm->setAvailable(available);
    }

    updateTable();
}

void LabOne::updateTable()
{
    allocationTable->setRowCount(allocation.size());
    allocationTable->setColumnCount(resourceCount);
    maxTable->setRowCount(max.size());
    maxTable->setColumnCount(resourceCount);
    needTable->setRowCount(need.size());
    needTable->setColumnCount(resourceCount);

    for (int i = 0; i < allocation.size(); ++i) {
        for (int j = 0; j < resourceCount; ++j) {
            allocationTable->setItem(i, j, new QTableWidgetItem(QString::number(allocation[i][j])));
            maxTable->setItem(i, j, new QTableWidgetItem(QString::number(max[i][j])));
            needTable->setItem(i, j, new QTableWidgetItem(QString::number(need[i][j])));
        }
    }

    availableTable->setRowCount(1);
    availableTable->setColumnCount(resourceCount);
    for (int i = 0; i < resourceCount; ++i) {
        availableTable->setItem(0, i, new QTableWidgetItem(QString::number(available[i])));
    }
}

bool LabOne::checkSafety()
{
    return bankersAlgorithm->checkSafety();
}
