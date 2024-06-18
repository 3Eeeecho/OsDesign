#ifndef LabFive_H
#define LabFive_H

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>

class LabFive : public QMainWindow
{
    Q_OBJECT

public:
    LabFive(QWidget *parent = nullptr);
    ~LabFive();

private slots:
    void generatePageReferences();
    void simulate();
    void setFrameCount();

private:
    void simulateFIFO();
    void simulateLRU();
    void simulateOPT();
    float calculateHitRate(const std::vector<int>& pageReferences, const std::vector<int>& frames);

    QComboBox *algorithmComboBox;
    QPushButton *generateButton;
    QPushButton *simulateButton;
    QPushButton *setFrameCountButton;
    QTextEdit *display;
    std::vector<int> pageReferences;
    int frameCount;

    void setupUI();
};

#endif // LabFive_H
