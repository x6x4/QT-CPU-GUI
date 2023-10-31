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

void GUI_State::operator() () {

    for (int i = 0; i < data_block.size(); i++) {
        data_block[i]->setText(QString::number(cpu->data(i)));
    }

    for (int i = 1; i < gpreg_block.size(); i+=2) {
        gpreg_block[i]->setText(QString::number(cpu->gp(i/2)));
    }

    spreg_block[1]->setText(QString::number(cpu->sp(0)));
    spreg_block[3]->setText(QString::number(cpu->sp(1)));
};


void Run_Button::on_click() {
    std::vector <std::size_t> bps;
    for (std::size_t i = 0; i < breakpoints->size(); i++) {
        if ((*breakpoints)[i]->get_clicked())
            bps.push_back(i);
        (*breakpoints)[i]->is_frozen = 1;
    }

    std::function<void()> f = *sections;

    exec(*(sections->cpu), bps, f);
}

