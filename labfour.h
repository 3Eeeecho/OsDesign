#ifndef LABFOUR_H
#define LABFOUR_H

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <vector>

struct Block {
    int size;
    bool free;
    int start;
};

class LabFour : public QMainWindow
{
    Q_OBJECT

public:
    LabFour(QWidget *parent = nullptr);
    ~LabFour();

private slots:
    void allocateMemory();
    void freeMemory();
    void updateDisplay();

private:
    void firstFit(int size);
    void nextFit(int size);
    void bestFit(int size);
    void deallocate(int start);
    QComboBox *algorithmComboBox;
    QPushButton *allocateButton;
    QPushButton *freeButton;
    QVBoxLayout *mainLayout;
    QLabel *statusLabel;
    QTextEdit *display;

    std::vector<Block> memory;
    int nextFitPointer;

    void setupUI();
    int getRemainingMemory();
};

#endif // LABFOUR_H
