#include "mainwindow.h"

//#include "/home/cracky/cppfun/3/lib/IR_compiler/fwd_IR_compiler.h"
#include "/home/cracky/cppfun/3/instr_set/instr_set.h"
#include "qapplication.h"
#include "qwidget.h"
#include <QFileDialog>
#include <QApplication>
#include <QPushButton>
#include <QTextStream>
#include <QLabel>
#include <QFile>
#include <QLineEdit>
#include <QGridLayout>
#include <cstddef>

int main(int argc, char *argv[])
{
    QApplication a = QApplication(argc, argv);

    QWidget win;
    std::size_t win_w = 700;
    win.setFixedSize(win_w, 500);
    win.setToolTip("CPU emulator");

    /*/  GENERAL UI  /*/

    std::size_t b_sz = 50, little_b_sz = 20;

    QPushButton *b_quit = new QPushButton ("Exit", &win);
    b_quit->setGeometry(win_w - b_sz - 10, 10, b_sz, b_sz);
    win.connect(b_quit, SIGNAL (clicked()), a.instance(), SLOT (quit()));

    Load_Button *b_load = new Load_Button (&win, new CPU(iset), b_sz, little_b_sz);
    b_load->setGeometry(b_quit->x() - 3*b_sz - b_sz - 10, b_quit->y(), 3*b_sz, b_sz);

    win.show();

    return a.exec();
}

