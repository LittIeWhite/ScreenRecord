#pragma execution_character_set("utf-8")
#include "ScreenRecord.h"
#include "SingleApplication.h"

int main(int argc, char *argv[])
{
    SingleApplication a("ScreenRecord", argc, argv);
    if (a.IsRunning())
    {
        QMessageBox::information(0, "Tips", "The app is already running!");
        return 0;
    }
    ScreenRecord w;
    w.show();
    return a.exec();
}
