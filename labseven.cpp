#include "labseven.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <algorithm>
#include <random>

LabSeven::LabSeven(QWidget *parent)
    : QMainWindow(parent), headPosition(0)
{
    setupUI();
}

LabSeven::~LabSeven()
{
}

void LabSeven::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    algorithmComboBox = new QComboBox(this);
    algorithmComboBox->addItem("FCFS");
    algorithmComboBox->addItem("SSTF");
    algorithmComboBox->addItem("SCAN");
    algorithmComboBox->addItem("CSCAN");

    generateButton = new QPushButton("生成访问序列", this);
    simulateButton = new QPushButton("模拟", this);
    compareButton = new QPushButton("比较算法", this);

    display = new QTextEdit(this);
    display->setReadOnly(true);

    mainLayout->addWidget(new QLabel("选择调度算法:", this));
    mainLayout->addWidget(algorithmComboBox);
    mainLayout->addWidget(generateButton);
    mainLayout->addWidget(simulateButton);
    mainLayout->addWidget(compareButton);
    mainLayout->addWidget(new QLabel("结果:", this));
    mainLayout->addWidget(display);

    setCentralWidget(centralWidget);

    connect(generateButton, &QPushButton::clicked, this, &LabSeven::generateSequence);
    connect(simulateButton, &QPushButton::clicked, this, &LabSeven::simulate);
    connect(compareButton, &QPushButton::clicked, this, &LabSeven::compareAlgorithms);

    setWindowTitle("实验七");
}

void LabSeven::generateSequence()
{
    bool ok;
    int sequenceLength = QInputDialog::getInt(this, "生成访问序列", "输入访问序列长度:", 10, 1, 100, 1, &ok);
    if (ok) {
        sequence.clear();
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 199);

        for (int i = 0; i < sequenceLength; ++i) {
            sequence.push_back(dis(gen));
        }

        QString sequenceStr = "访问序列: ";
        for (int pos : sequence) {
            sequenceStr += QString::number(pos) + " ";
        }
        display->setText(sequenceStr);
    }
}

void LabSeven::compareAlgorithms()
{
    if (sequence.empty()) {
        QMessageBox::warning(this, "警告", "请先生成访问序列！");
        return;
    }

    bool ok;
    int initialHeadPosition = QInputDialog::getInt(this, "设置磁头初始位置", "输入磁头初始位置:", 0, 0, 199, 1, &ok);
    if (!ok) return;

    results.clear();

    simulateFCFS(sequence, initialHeadPosition);
    simulateSSTF(sequence, initialHeadPosition);
    simulateSCAN(sequence, initialHeadPosition);
    simulateCSCAN(sequence, initialHeadPosition);

    displayComparison();
}

void LabSeven::simulateFCFS(const std::vector<int>& sequence, int initialHeadPosition)
{
    int totalSeekLength = 0;
    int currentPosition = initialHeadPosition;
    seekSteps.clear();
    seekSteps.push_back(QString::number(currentPosition));

    for (int request : sequence) {
        totalSeekLength += std::abs(currentPosition - request);
        seekSteps.push_back(QString::number(request));
        currentPosition = request;
    }

    float averageSeekLength = static_cast<float>(totalSeekLength) / sequence.size();
    results.push_back({"FCFS", averageSeekLength});
}

void LabSeven::simulateSSTF(const std::vector<int>& sequence, int initialHeadPosition)
{
    std::vector<int> tempSequence = sequence;
    int totalSeekLength = 0;
    int currentPosition = initialHeadPosition;
    seekSteps.clear();
    seekSteps.push_back(QString::number(currentPosition));

    while (!tempSequence.empty()) {
        auto closest = std::min_element(tempSequence.begin(), tempSequence.end(),
                                        [currentPosition](int a, int b) {
                                            return std::abs(a - currentPosition) < std::abs(b - currentPosition);
                                        });
        totalSeekLength += std::abs(currentPosition - *closest);
        seekSteps.push_back(QString::number(*closest));
        currentPosition = *closest;
        tempSequence.erase(closest);
    }

    float averageSeekLength = static_cast<float>(totalSeekLength) / sequence.size();
    results.push_back({"SSTF", averageSeekLength});
}


void LabSeven::simulateSCAN(const std::vector<int>& sequence, int initialHeadPosition)
{
    std::vector<int> tempSequence = sequence;
    tempSequence.push_back(initialHeadPosition);
    std::sort(tempSequence.begin(), tempSequence.end());

    auto it = std::find(tempSequence.begin(), tempSequence.end(), initialHeadPosition);
    int totalSeekLength = 0;
    int currentPosition = initialHeadPosition;
    seekSteps.clear();
    seekSteps.push_back(QString::number(currentPosition));

    for (auto i = it; i != tempSequence.end(); ++i) {
        totalSeekLength += std::abs(currentPosition - *i);
        seekSteps.push_back(QString::number(*i));
        currentPosition = *i;
    }
    for (auto i = it; i != tempSequence.begin(); --i) {
        totalSeekLength += std::abs(currentPosition - *(i - 1));
        seekSteps.push_back(QString::number(*(i - 1)));
        currentPosition = *(i - 1);
    }

    float averageSeekLength = static_cast<float>(totalSeekLength) / sequence.size();
    results.push_back({"SCAN", averageSeekLength});
}

void LabSeven::simulateCSCAN(const std::vector<int>& sequence, int initialHeadPosition)
{
    std::vector<int> tempSequence = sequence;
    tempSequence.push_back(initialHeadPosition);
    std::sort(tempSequence.begin(), tempSequence.end());

    auto it = std::find(tempSequence.begin(), tempSequence.end(), initialHeadPosition);
    int totalSeekLength = 0;
    int currentPosition = initialHeadPosition;
    seekSteps.clear();
    seekSteps.push_back(QString::number(currentPosition));

    for (auto i = it; i != tempSequence.end(); ++i) {
        totalSeekLength += std::abs(currentPosition - *i);
        seekSteps.push_back(QString::number(*i));
        currentPosition = *i;
    }
    if (it != tempSequence.begin()) {
        totalSeekLength += std::abs(currentPosition - *tempSequence.begin());
        seekSteps.push_back(QString::number(*tempSequence.begin()));
        currentPosition = *tempSequence.begin();
        for (auto i = tempSequence.begin(); i != it; ++i) {
            totalSeekLength += std::abs(currentPosition - *i);
            seekSteps.push_back(QString::number(*i));
            currentPosition = *i;
        }
    }

    float averageSeekLength = static_cast<float>(totalSeekLength) / sequence.size();
    results.push_back({"CSCAN", averageSeekLength});
}

void LabSeven::simulate()
{
    if (sequence.empty()) {
        QMessageBox::warning(this, "警告", "请先生成访问序列！");
        return;
    }

    bool ok;
    headPosition = QInputDialog::getInt(this, "设置磁头初始位置", "输入磁头初始位置:", 0, 0, 199, 1, &ok);
    if (!ok) return;

    results.clear();
    seekSteps.clear();

    QString algorithm = algorithmComboBox->currentText();
    if (algorithm == "FCFS") {
        simulateFCFS(sequence, headPosition);
    } else if (algorithm == "SSTF") {
        simulateSSTF(sequence, headPosition);
    } else if (algorithm == "SCAN") {
        simulateSCAN(sequence, headPosition);
    } else if (algorithm == "CSCAN") {
        simulateCSCAN(sequence, headPosition);
    }

    displaySeekSteps(); // 显示寻道过程
    display->append(QString("\n%1 平均寻道长度: %2").arg(algorithm).arg(results[0].averageSeekLength, 0, 'f', 2));
}

void LabSeven::displaySeekSteps()
{
    display->append("\n寻道过程:");
    if (!seekSteps.empty()) {
        QStringList stepList;
        for (const auto& step : seekSteps) {
            stepList.append(step);
        }
        display->append(stepList.join("->"));
    }
}




void LabSeven::displayComparison()
{
    display->append("\n比较结果:");
    for (const auto& result : results) {
        display->append(QString("%1 平均寻道长度: %2").arg(result.name).arg(result.averageSeekLength, 0, 'f', 2));
    }
}



