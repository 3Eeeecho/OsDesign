#ifndef HOME_H
#define HOME_H

#include <QWidget>
#include <QProcess>
#include "labone.h"
#include "labtwo.h"
#include "labthree.h"
#include "labfour.h"
#include "labfive.h"
#include "labsix.h"
#include "labseven.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Home;
}
QT_END_NAMESPACE

class Home : public QWidget
{
    Q_OBJECT

public:
    Home(QWidget *parent = nullptr);
    ~Home();

private slots:
    void on_labOneButton_clicked();

    void on_labTwoButton_clicked();

    void on_labThreeButton_clicked();

    void on_labFourButton_clicked();

    void on_labFiveButton_clicked();

    void on_labSixButton_clicked();

    void on_labSevenButton_clicked();

    void on_labEightButton_clicked();

private:
    Ui::Home *ui;
    QProcess *process;
    LabOne l1;
    LabTwo l2;
    LabThree l3;
    LabFour l4;
    LabFive l5;
    LabSix l6;
    LabSeven l7;
};
#endif // HOME_H
