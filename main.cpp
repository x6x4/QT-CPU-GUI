#include "mainwindow.h"
#include "qwidget.h"

int main(int argc, char *argv[])
{
    QApplication a = QApplication(argc, argv);
    QWidget mainW;
    mainW.setFixedSize(700, 700);
    mainW.setToolTip("CPU emulator");
    mainW.show();

    auto main = Controller(a, &mainW);


    return a.exec();
}

