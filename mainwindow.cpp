#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "qfiledialog.h"
#include "qpushbutton.h"
#include "qwidget.h"
#include <QDialog>
#include <QGridLayout>
#include <QDebug>
#include "/home/cracky/cppfun/3/lib/IR_compiler/fwd_IR_compiler.h"

/*  OWN CLASSES  */

void GUI_State::operator() () {
    update();
    if (!cpu->is_over()) RunDialog(run_button).exec();
};

RunDialog::RunDialog(QPushButton* orig) : QDialog(orig->parentWidget()) {
    setWindowFlags(Qt::Widget);
    this->setGeometry(orig->geometry());

    QPushButton *copy = new QPushButton ("Stopped", this);
    copy->setStyleSheet("background-color: #BD8F31");
    copy->setFixedSize(orig->size());

    connect(copy, &QPushButton::clicked, this, &QDialog::close);
}

//  SECTIONS UPDATES

void GUI_State::update() { for (auto sect : vec) sect->update(); }

void Data_Widget::update () {
    for (int i = 0; i < main_block.size(); i++) {
        main_block[i]->setText(QString::number(cpu->data_cell(i)));
    }
}

void GPREG_Widget::update () {
    for (int i = 1; i < main_block.size(); i+=2) {
        main_block[i]->setText(QString::number(cpu->gp(i/2)));
    }
}

void SPREG_Widget::update () {
    for (int i = 1; i < main_block.size(); i+=2) {
        main_block[i]->setText(QString::number(cpu->sp(i/2)));
    }
}

CPU_Widgets::CPU_Widgets (QWidget *parent, CPU *cpu, std::size_t b_sz, std::size_t little_b_sz) {

    auto lines = cpu->lines();

    std::size_t sections_y = 70, sections_w = 100;

    code_win = new Code_Widget (parent, lines, little_b_sz);
    code_win->move(little_b_sz, sections_y+b_sz + 10);

    _data = new Data_Widget(parent, cpu);
    _data->setGeometry(little_b_sz, sections_y, 300, 50);

    gpreg = new GPREG_Widget(parent, cpu);
    gpreg->setGeometry(350, sections_y+b_sz + 10, sections_w, 300);

    spreg = new SPREG_Widget(parent, cpu);
    spreg->setGeometry(350+sections_w+10, sections_y+b_sz + 10, sections_w, 100);

    b_run = new Run_Button(parent, code_win->bps(), GUI_State({_data, gpreg, spreg}, cpu));
    b_run->setGeometry(30, 10, b_sz, b_sz);

    parent->show();
}

//  BUTTONS

void Run_Button::on_click() {

    //  get bps and froze them
    std::vector <std::size_t> bps;
    for (std::size_t i = 0; i < breakpoints.size(); i++) {
        if (breakpoints[i]->get_clicked())
            bps.push_back(i);
        breakpoints[i]->is_frozen = 1;
    }

    show_state.run_button = this;
    exec(*show_state.cpu, bps, show_state);

    //  unfroze bps
    for (std::size_t i = 0; i < breakpoints.size(); i++) {
        breakpoints[i]->is_frozen = 0;
    }
}

Run_Button::Run_Button(QWidget *parent, std::vector<Breakpoint_Button*> bps, GUI_State _sections)
    : Click_Button(parent), breakpoints(bps), show_state(_sections) {
    setText("Run");
    setStyleSheet("background-color: #F72626");
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

Open_Button::Open_Button (QWidget *parent, CPU *_cpu, std::size_t _b_sz, std::size_t _little_b_sz)
    : QPushButton (parent), cpu (_cpu), b_sz (_b_sz), little_b_sz (_little_b_sz) {
    setText("Load program");
    parent->connect(this, SIGNAL (clicked()), this, SLOT (load()));
}

void Open_Button::load () {
    QString filename = QFileDialog(parentWidget()).getOpenFileName(parentWidget(),"Open",
                                                "/home/cracky/cppfun/3/", "Asm files (*.asm)");
    if (!filename.size()) return;

    if (cpu) {
        load_cpu(*cpu, filename.toStdString().c_str());
        gui = new CPU_Widgets(parentWidget(), cpu, b_sz, little_b_sz);
        parentWidget()->show();
    }
}

//  LABELS

Styled_Label::Styled_Label(QWidget *parent)
    : QLabel(parent) {
    setFrameStyle(QFrame::StyledPanel);
    setAlignment(Qt::AlignCenter);
}

//  SECTIONS WIDGETS CTORS

Code_Widget::Code_Widget (QWidget *parent, strings raw_lines, std::size_t bp_sz)
    : QWidget (parent), code_lines (raw_lines) {
    QGridLayout *cw_grid = new QGridLayout(this);

    for (int i = 0; i < code_lines.size(); i++) {
        Breakpoint_Button *bp = new Breakpoint_Button (this, i+1);
        bp->setFixedSize(bp_sz, bp_sz);
        breakpoints.push_back(bp);

        cw_grid->addWidget(bp, i,0);

        code_lines[i]->setFixedHeight(bp_sz);
        code_lines[i]->setFixedWidth(100);
        cw_grid->addWidget(code_lines[i], i,1, 1, -1);
    }

    cw_grid->deleteLater();
};

Data_Widget::Data_Widget (QWidget *parent, CPU *cpu) : Section_Widget (parent, cpu) {

    Styled_Label *data_header = new Styled_Label(parent);
    data_header->setText("DATA");
    data_header->setStyleSheet("background-color: #8EDC7A");

    main_block = Cell_Block(cpu->data_sz(), parent);

    QGridLayout *data_grid = new QGridLayout(this);
    data_grid->addWidget(data_header, 0,0, 1,2);

    for (int i = 0; i < main_block.size(); i++) {
        data_grid->addWidget(main_block[i], 0, i+2);

        if (i < cpu->data_sz())
            main_block[i]->setText(QString::number(cpu->data_cell(i)));
        else
            main_block[i]->setText("0");
    }
};

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

        main_block[i]->setText(QString("r%1").arg(i/2));
        main_block[i+1]->setText("0");
    }
};

SPREG_Widget::SPREG_Widget (QWidget *parent, CPU *cpu) : Section_Widget (parent, cpu) {

    Styled_Label *sp_reg_header = new Styled_Label(parent);
    sp_reg_header->setText("SP REGS");
    sp_reg_header->setStyleSheet("background-color: #DAD59C");

    QGridLayout *sp_grid = new QGridLayout(this);
    sp_grid->addWidget(sp_reg_header, 0,0, 1,2);

    main_block = Cell_Block(2*2, parent);

    for (int i = 0; i < 2*2; i+=2) {
        sp_grid->addWidget(main_block[i], i+1, 0);
        sp_grid->addWidget(main_block[i+1], i+1, 1);

        main_block[i+1]->setText("0");
    }

    main_block[0]->setText("pc");
    main_block[2]->setText("zf");
}

//  DEFAULT CLASSES

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
