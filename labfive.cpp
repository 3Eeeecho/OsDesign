#include "labfive.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <random>
#include <algorithm>

LabFive::LabFive(QWidget *parent)
    : QMainWindow(parent), frameCount(3)
{
    setupUI();
}

LabFive::~LabFive()
{
}

void LabFive::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    algorithmComboBox = new QComboBox(this);
    algorithmComboBox->addItem("FIFO");
    algorithmComboBox->addItem("LRU");
    algorithmComboBox->addItem("OPT");

    generateButton = new QPushButton("生成页面走向", this);
    simulateButton = new QPushButton("模拟", this);
    setFrameCountButton = new QPushButton("设置页框", this);

    display = new QTextEdit(this);
    display->setReadOnly(true);

    mainLayout->addWidget(new QLabel("选择置换算法:", this));
    mainLayout->addWidget(algorithmComboBox);
    mainLayout->addWidget(setFrameCountButton);
    mainLayout->addWidget(generateButton);
    mainLayout->addWidget(simulateButton);
    mainLayout->addWidget(new QLabel("结果:", this));
    mainLayout->addWidget(display);


    setCentralWidget(centralWidget);

    connect(generateButton, &QPushButton::clicked, this, &LabFive::generatePageReferences);
    connect(simulateButton, &QPushButton::clicked, this, &LabFive::simulate);
    connect(setFrameCountButton, &QPushButton::clicked, this, &LabFive::setFrameCount);

    setWindowTitle("实验五");
}

void LabFive::setFrameCount()
{
    bool ok;
    int count = QInputDialog::getInt(this, "设置页框", "输入页框:", frameCount, 1, 100, 1, &ok);
    if (ok) {
        frameCount = count;
        display->append("页框已设置为: " + QString::number(frameCount));
    }
}

void LabFive::generatePageReferences()
{
    bool ok;
    int pageCount = QInputDialog::getInt(this, "生成页面走向", "输入页面走向长度:", 20, 1, 100, 1, &ok);
    if (ok) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 9);
        pageReferences.clear();

        for (int i = 0; i < pageCount; ++i) {
            pageReferences.push_back(dis(gen));
        }

        QString pageReferenceStr = "页面走向: ";
        for (int page : pageReferences) {
            pageReferenceStr += QString::number(page) + " ";
        }
        display->append(pageReferenceStr);
    }
}

void LabFive::simulate()
{
    if (pageReferences.empty()) {
        QMessageBox::warning(this, "警告", "请先生成页面走向！");
        return;
    }

    QString algorithm = algorithmComboBox->currentText();
    if (algorithm == "FIFO") {
        simulateFIFO();
    } else if (algorithm == "LRU") {
        simulateLRU();
    } else if (algorithm == "OPT") {
        simulateOPT();
    }
}

void LabFive::simulateFIFO()
{
    std::vector<int> frames(frameCount, -1);
    int hitCount = 0;
    int nextFrame = 0;

    for (int page : pageReferences) {
        if (std::find(frames.begin(), frames.end(), page) != frames.end()) {
            hitCount++;
        } else {
            frames[nextFrame] = page;
            nextFrame = (nextFrame + 1) % frameCount;
        }
    }

    float hitRate = static_cast<float>(hitCount) / pageReferences.size();
    display->append("\nFIFO 命中率: " + QString::number(hitRate * 100, 'f', 2) + "%");
}

void LabFive::simulateLRU()
{
    std::vector<int> frames;
    std::vector<int> lruCounter(frameCount, 0);
    int hitCount = 0;
    int time = 0;

    for (int page : pageReferences) {
        auto it = std::find(frames.begin(), frames.end(), page);
        if (it != frames.end()) {
            hitCount++;
            lruCounter[it - frames.begin()] = time++;
        } else {
            if (frames.size() < frameCount) {
                frames.push_back(page);
                lruCounter[frames.size() - 1] = time++;
            } else {
                auto lruIt = std::min_element(lruCounter.begin(), lruCounter.end());
                int lruIndex = lruIt - lruCounter.begin();
                frames[lruIndex] = page;
                lruCounter[lruIndex] = time++;
            }
        }
    }

    float hitRate = static_cast<float>(hitCount) / pageReferences.size();
    display->append("\nLRU 命中率: " + QString::number(hitRate * 100, 'f', 2) + "%");
}

void LabFive::simulateOPT()
{
    std::vector<int> frames;
    int hitCount = 0;

    for (size_t i = 0; i < pageReferences.size(); ++i) {
        int page = pageReferences[i];
        if (std::find(frames.begin(), frames.end(), page) != frames.end()) {
            hitCount++;
        } else {
            if (frames.size() < frameCount) {
                frames.push_back(page);
            } else {
                int furthestIndex = -1;
                int frameToReplace = 0;
                for (size_t j = 0; j < frames.size(); ++j) {
                    int nextUse = std::find(pageReferences.begin() + i + 1, pageReferences.end(), frames[j]) - pageReferences.begin();
                    if (nextUse > furthestIndex) {
                        furthestIndex = nextUse;
                        frameToReplace = j;
                    }
                }
                frames[frameToReplace] = page;
            }
        }
    }

    float hitRate = static_cast<float>(hitCount) / pageReferences.size();
    display->append("\nOPT 命中率: " + QString::number(hitRate * 100, 'f', 2) + "%");
}
