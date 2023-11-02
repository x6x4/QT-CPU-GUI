#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "qgridlayout.h"
#include <QEventLoop>


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


//  SECTIONS WIDGETS

Data_Widget::Data_Widget (QWidget *parent, std::unique_ptr<Data> data, CPU *cpu) : Section_Widget (parent, cpu) {

    Styled_Label *data_header = new Styled_Label(parent);
    data_header->setText("DATA");
    data_header->setStyleSheet("background-color: #8EDC7A");

    main_block = Cell_Block(gui_cpu->data_sz(), parent);

    QGridLayout *data_grid = new QGridLayout(this);
    data_grid->addWidget(data_header, 0,0, 1,2);

    for (int i = 0; i < main_block.size(); i++) {
        data_grid->addWidget(main_block[i], 0, i+2);

        if (data && i < data->size())
            main_block[i]->setText(QString::number((*data).at(i)));
        else
            main_block[i]->setText("0");
    }
};

void Data_Widget::update () {
    for (int i = 0; i < main_block.size(); i++) {
        main_block[i]->setText(QString::number(gui_cpu->data(i)));
    }
}

GPREG_Widget::GPREG_Widget (QWidget *parent, CPU *cpu) : Section_Widget (parent, cpu) {

    Styled_Label *gp_reg_header = new Styled_Label(parent);
    gp_reg_header->setText("GP REGS");
    gp_reg_header->setStyleSheet("background-color: #BDAEAE");

    QGridLayout *gp_grid = new QGridLayout(this);
    gp_grid->addWidget(gp_reg_header, 0,0, 1,2);

    main_block = Cell_Block(8*2, parent);

    for (int i = 0; i < main_block.size(); i+=2) {
        gp_grid->addWidget(main_block[i], i+1, 0);
        gp_grid->addWidget(main_block[i+1], i+1, 1);

        QVector<QString> reg_label{"r", QString::number(i/2)};
        const QString reg = std::accumulate(reg_label.cbegin(),reg_label.cend(), QString{});
        main_block[i]->setText(reg);

        main_block[i+1]->setText("0");
    }
};

void GPREG_Widget::update () {
    for (int i = 1; i < main_block.size(); i+=2) {
        main_block[i]->setText(QString::number(gui_cpu->gp(i/2)));
    }
}

//  BUTTONS

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

    spreg_block[1]->setText(QString::number(cpu->sp(0)));
    spreg_block[3]->setText(QString::number(cpu->sp(1)));

    run_button->is_clicked = 0;
    run_button->setStyleSheet("background-color: #BD8F31");
    run_button->setText("Stopped");

    QEventLoop click_run;
    std::cout << run_button->parent()->connect(run_button, SIGNAL (clicked()), &click_run, SLOT (quit())) << std::endl;
    click_run.exec();

    run_button->setText("Run");
    run_button->setStyleSheet("background-color: #F72626");
};

void CPU_Thread::update () {
    //run_button->state();
}

void CPU_Thread::run () {



    //std::function<void()> f = *state;

    //exec(*(state->cpu), bps, f);

    // Create event loop
    QEventLoop loop;

    // Connect some signals and slots
    run_button->parent()->connect(run_button, SIGNAL (clicked()), &loop, SLOT (quit()));

    // Start event loop
    loop.exec();
    //emit quit();

    // Continue execution of thread
    // ...
}


void Run_Button::on_click() {

    //  get bps and froze them
    std::vector <std::size_t> bps;
    for (std::size_t i = 0; i < breakpoints->size(); i++) {
        if ((*breakpoints)[i]->get_clicked())
            bps.push_back(i);
        (*breakpoints)[i]->is_frozen = 1;
    }

    CPU_Thread cthread (this);
}

