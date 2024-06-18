#include <QApplication>
#include "childprocess.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ChildProcess child;
    child.start();

    return a.exec();
}
