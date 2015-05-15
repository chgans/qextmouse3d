#include "mainwindow.h"
#include <QApplication>
#include <QPluginLoader>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QPluginLoader loader("../src/plugins/mouse3d/linuxinput/libqmouse3dlinuxinput.so");
    qDebug() << loader.errorString();
    qDebug() << loader.load();
    qDebug() << loader.errorString();
    return a.exec();
}
