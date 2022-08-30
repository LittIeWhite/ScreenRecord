#include "ScreenRecord.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ScreenRecord w;
    w.show();
    return a.exec();
}
