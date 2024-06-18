#ifndef LabOne_H
#define LabOne_H

#include <QMainWindow>
#include <QVector>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include "bankersalgorithm.h"

class LabOne : public QMainWindow
{
    Q_OBJECT

public:
    LabOne(QWidget *parent = nullptr);
    ~LabOne();

private slots:
    void onAddProcess();
    void onRequestResource();
    void onLoadSampleData();
    void onCheckSafety();
    void onSetInitialResources();

private:
    void setupUI();
    void updateTable();
    bool checkSafety();

    QSpinBox *resourceCountSpinBox;
    QSpinBox *processCountSpinBox;
    QTableWidget *allocationTable;
    QTableWidget *maxTable;
    QTableWidget *needTable;
    QTableWidget *availableTable;
    QPushButton *checkSafetyButton;
    QPushButton *setInitialResourcesButton;

    QVector<QVector<int>> allocation;
    QVector<QVector<int>> max;
    QVector<QVector<int>> need;
    QVector<int> available;

    int resourceCount;
    int processCount;

    BankersAlgorithm *bankersAlgorithm;
};

#endif // LabOne_H
