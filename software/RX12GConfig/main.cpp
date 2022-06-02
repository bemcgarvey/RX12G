#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setOrganizationName("QES");
    QApplication::setApplicationName("RX12GConfig");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
