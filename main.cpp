#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/notes.png"));
    QCoreApplication::setOrganizationName("Genavir");
    QCoreApplication::setApplicationName("LogBook");
    MainWindow w;
    w.show();
    return a.exec();
}
