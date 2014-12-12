#include <QApplication>
#include "mainwindow.h"
#include "hit.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<Hit>();

    MainWindow w;
    w.show();

    return a.exec();
}
