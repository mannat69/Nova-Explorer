#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setOrganizationName("FileManagerProject");
    QApplication::setApplicationName("FileManager");
    MainWindow w;
    w.show();
    return QApplication::exec();
}
