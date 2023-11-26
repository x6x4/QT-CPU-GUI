#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a = QApplication(argc, argv);
    auto main = Controller(a);
    main.show();

    return a.exec();
}

