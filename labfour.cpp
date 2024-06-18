#include "LabFour.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QInputDialog>
#include <QMessageBox>

LabFour::LabFour(QWidget *parent)
    : QMainWindow(parent), nextFitPointer(0)
{
    setupUI();
    memory.push_back({700, true, 0}); // Initialize memory with a single large free block
    updateDisplay();
}

LabFour::~LabFour()
{
}

void LabFour::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    mainLayout = new QVBoxLayout(centralWidget);

    algorithmComboBox = new QComboBox(this);
    algorithmComboBox->addItem("首次适应");
    algorithmComboBox->addItem("循环首次适应");
    algorithmComboBox->addItem("最佳适应");

    allocateButton = new QPushButton("分配内存", this);
    freeButton = new QPushButton("释放内存", this);

    statusLabel = new QLabel("内存状态:", this);
    display = new QTextEdit(this);
    display->setReadOnly(true);

    mainLayout->addWidget(new QLabel("选择分配算法:", this));
    mainLayout->addWidget(algorithmComboBox);
    mainLayout->addWidget(allocateButton);
    mainLayout->addWidget(freeButton);
    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(display);

    setCentralWidget(centralWidget);
    setWindowTitle("实验四");

    connect(allocateButton, &QPushButton::clicked, this, &LabFour::allocateMemory);
    connect(freeButton, &QPushButton::clicked, this, &LabFour::freeMemory);
}

void LabFour::allocateMemory()
{
    bool ok;
    int size = QInputDialog::getInt(this, "分配内存", "输入分配内存大小:", 0, 1, getRemainingMemory(), 1, &ok);
    if (ok) {
        if (algorithmComboBox->currentText() == "首次适应") {
            firstFit(size);
        } else if (algorithmComboBox->currentText() == "循环首次适应") {
            nextFit(size);
        } else if (algorithmComboBox->currentText() == "最佳适应") {
            bestFit(size);
        }
        updateDisplay();
    }
}

void LabFour::freeMemory()
{
    bool ok;
    int start = QInputDialog::getInt(this, "释放内存", "输入释放内存首地址:", 0, 0, 700, 1, &ok);
    if (ok) {
        deallocate(start);
        updateDisplay();
    }
}

void LabFour::firstFit(int size)
{
    for (auto it = memory.begin(); it != memory.end(); ++it) {
        Block &block = *it;
        if (block.free && block.size >= size) {
            // 记录当前空闲块的起始地址
            int newStart = block.start;

            // 如果当前块大小大于需要分配的大小，拆分块
            if (block.size > size) {
                block.start += size;
                block.size -= size;
                // 插入新块描述器
                memory.insert(it, {size, false, newStart});
            } else {
                // 如果块大小正好等于需要分配的大小，直接分配
                block.free = false;
            }
            return;
        }
    }
    QMessageBox::warning(this, "分配失败", "没有合适的内存块分配!");
}

void LabFour::nextFit(int size)
{
    int startPointer = nextFitPointer;
    auto memorySize = memory.size();

    do {
        Block &block = memory[nextFitPointer];
        if (block.free && block.size >= size) {
            // 记录当前空闲块的起始地址
            int newStart = block.start;

            // 如果当前块大小大于需要分配的大小，拆分块
            if (block.size > size) {
                block.start += size;
                block.size -= size;
                // 插入新块描述器
                memory.insert(memory.begin() + nextFitPointer, {size, false, newStart});
            } else {
                // 如果块大小正好等于需要分配的大小，直接分配
                block.free = false;
            }

            // 更新 nextFitPointer
            nextFitPointer = (nextFitPointer + 1) % memorySize;
            return;
        }

        // 更新 nextFitPointer，确保不会越界
        nextFitPointer = (nextFitPointer + 1) % memorySize;

    } while (nextFitPointer != startPointer);

    // 如果循环了一圈仍然没有找到合适的块，提示分配失败
    QMessageBox::warning(this, "分配失败", "没有合适的内存块分配!");
}

void LabFour::bestFit(int size)
{
    int bestIndex = -1;
    int bestSize = INT_MAX;

    // 查找最适合的空闲块
    for (size_t i = 0; i < memory.size(); ++i) {
        if (memory[i].free && memory[i].size >= size && memory[i].size < bestSize) {
            bestIndex = i;
            bestSize = memory[i].size;
        }
    }

    if (bestIndex != -1) {
        Block &block = memory[bestIndex];
        // 记录当前空闲块的起始地址
        int newStart = block.start;

        // 如果当前块大小大于需要分配的大小，拆分块
        if (block.size > size) {
            block.start += size;
            block.size -= size;
            // 插入新块描述器
            memory.insert(memory.begin() + bestIndex, {size, false, newStart});
        } else {
            // 如果块大小正好等于需要分配的大小，直接分配
            block.free = false;
        }
    } else {
        QMessageBox::warning(this, "分配失败", "没有合适的内存块分配!");
    }
}

void LabFour::deallocate(int start)
{
    for (auto it = memory.begin(); it != memory.end(); ++it) {
        if (it->start == start && !it->free) {
            it->free = true;

            // 合并前面
            if (it != memory.begin()) {
                auto prev = std::prev(it);
                if (prev->free) {
                    prev->size += it->size;
                    it = memory.erase(it);
                    it = prev;
                }
            }

            // 合并后面
            if (std::next(it) != memory.end()) {
                auto next = std::next(it);
                if (next->free) {
                    it->size += next->size;
                    memory.erase(next);
                }
            }

            return;
        }
    }
    QMessageBox::warning(this, "回收失败", "没有在首地址找到要回收的内存块!");
}

void LabFour::updateDisplay()
{
    display->clear();
    int totalFreeMemory = getRemainingMemory();
    for (const auto &block : memory) {
        display->append(QString("首地址: %1, 大小: %2, %3").arg(block.start).arg(block.size).arg(block.free ? "空闲" : "已分配"));
    }
    display->append(QString("\n剩余空闲空间: %1").arg(totalFreeMemory));
}

int LabFour::getRemainingMemory()
{
    int remainingMemory = 0;
    for (const auto &block : memory) {
        if (block.free) {
            remainingMemory += block.size;
        }
    }
    return remainingMemory;
}
