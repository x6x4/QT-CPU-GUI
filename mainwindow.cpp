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

Styled_Label::Styled_Label(QWidget *parent)
    : QLabel(parent) {
    setFrameStyle(QFrame::StyledPanel);
    setAlignment(Qt::AlignCenter);
}

Click_Button::Click_Button(QWidget *parent)
    : QPushButton(parent) {
    is_clicked = 0;
    connect(this, &QPushButton::clicked, this, &Click_Button::on_click);
}

void Breakpoint_Button::on_click() {
    if (is_frozen) return;

    if (Click_Button::is_clicked) {
        setStyleSheet("background-color: #FFFFFF");
        Click_Button::is_clicked = 0;
    }
    else {
        setStyleSheet("background-color: #F72626");
        Click_Button::is_clicked = 1;
    }
}

void word () { std::cout << "Word" << std::endl; }

void Run_Button::on_click() {
    std::vector <std::size_t> bps;
    for (std::size_t i = 0; i < breakpoints->size(); i++) {
        if ((*breakpoints)[i]->get_clicked())
            bps.push_back(i);
        (*breakpoints)[i]->is_frozen = 1;
    }

    cpu->exec(bps, word);
}

