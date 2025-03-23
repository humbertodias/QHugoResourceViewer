#include "hugo/hugoviewer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    hugo::HugoViewer w;
    w.show();
    return a.exec();
}
