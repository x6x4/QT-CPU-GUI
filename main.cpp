#include "mainwindow.h"
#include "qapplication.h"
#include "qwidget.h"

int main(int argc, char *argv[])
{
    QApplication a = QApplication(argc, argv);
    QWidget mainW;
    mainW.setFixedSize(700, 700);
    //  dark theme? not now
    //mainW.setStyleSheet("background-color: #404040");
    mainW.show();

    auto main = UIController(a, &mainW);

    return a.exec();
}

