#include "widget.h"

#include <QApplication>
#include <QDebug>
#include <QByteArray>
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Widget w;

    //HWND current = (HWND)w.winId();
    //SetParent(current,background);
    w.show();
    return a.exec();
}
