#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "/home/cracky/cppfun/3/lib/internals/cpu/cpu.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*  OWN CLASSES  */

Click_Button::Click_Button(QWidget *parent)
    : QPushButton(parent) {
    is_clicked = 0;
    connect(this, &QPushButton::clicked, this, &Click_Button::on_click);
}

void Breakpoint_Button::on_click() {
    if (Click_Button::is_clicked) {
        setStyleSheet("background-color: #FFFFFF");
        Click_Button::is_clicked = 0;
    }
    else {
        setStyleSheet("background-color: #F72626");
        Click_Button::is_clicked = 1;
    }
}

void Run_Button::on_click() {
    std::vector <std::size_t> bps;
    for (std::size_t i = 0; i < breakpoints->size(); i++) {
        if ((*breakpoints)[i]->get_clicked())
            bps.push_back(i);
    }

    cpu->exec();
}

