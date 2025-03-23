#include "hugoviewer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HugoViewer w;
    w.show();
    return a.exec();
}
