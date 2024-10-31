#include "conf.h"
#include "mainwindow.h"

#include <QApplication>


int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    Conf conf;
    w.show();
    return a.exec();
}
