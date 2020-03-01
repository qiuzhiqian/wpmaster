#include "widget.h"

#include <QApplication>
#include "hwndnode.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    HWNDNode *root=new HWNDNode();
    root->Sync();

    Widget w;
    w.show();
    return a.exec();
}
