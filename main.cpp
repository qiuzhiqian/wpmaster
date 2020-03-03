#include "widget.h"

#include <QApplication>
#include <QDebug>
#include <QByteArray>
#include <QThread>

Widget* w=nullptr;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    w=new Widget();

    w->show();
    return a.exec();
}
