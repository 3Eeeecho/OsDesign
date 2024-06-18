#ifndef LABSEVEN_H
#define LABSEVEN_H

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <vector>
#include <QString>

class LabSeven : public QMainWindow
{
    Q_OBJECT

public:
    LabSeven(QWidget *parent = nullptr);
    ~LabSeven();

private slots:
    void generateSequence();
    void simulate();
    void compareAlgorithms();

private:
    void setupUI();
    void simulateFCFS(const std::vector<int>& sequence, int initialHeadPosition);
    void simulateSSTF(const std::vector<int>& sequence, int initialHeadPosition);
    void simulateSCAN(const std::vector<int>& sequence, int initialHeadPosition);
    void simulateCSCAN(const std::vector<int>& sequence, int initialHeadPosition);
    void calculateAverageSeekLength(const std::vector<int>& sequence, const QString& algorithm, int initialHeadPosition);
    void displayComparison();
    void displaySeekSteps();

    QComboBox *algorithmComboBox;
    QPushButton *generateButton;
    QPushButton *simulateButton;
    QPushButton *compareButton;
    QTextEdit *display;

    std::vector<int> sequence;
    int headPosition;

    std::vector<QString> seekSteps;

    struct Result {
        QString name;
        float averageSeekLength;
    };
    std::vector<Result> results;
};

#endif // LABSEVEN_H
