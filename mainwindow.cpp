#include "mainwindow.h"
#include "qapplication.h"
#include "qdebug.h"
#include "qwidget.h"
#include <QDialog>
#include <QDebug>
#include <cstddef>
#include <functional>
#include <stdexcept>


void CPUState::load (strings s) {
    auto mcode = file_to_mcode(cpu.iSet(), s, avl_bps);
    cpu.load_mem(std::move(mcode));
}

View::View(QWidget *par, CPUState *_cpuState, strings lines) {

    std::size_t sections_y = 70, sections_w = 100;

    code_win = new Code_Widget (par);
    code_win->move(LB_SZ, sections_y+B_SZ + 10);
    code_win->init(lines);
    code_win->show();

    _data = new Data_Widget(par);
    _data->init(_cpuState->getCPU());
    _data->setGeometry(LB_SZ, sections_y, 300, 50);
    _data->show();

    gpreg = new GPREG_Widget(par);
    gpreg->init();
    gpreg->setGeometry(350, sections_y + 10, sections_w, 300);
    gpreg->show();

    spreg = new SPREG_Widget(par);
    spreg->init();
    spreg->setGeometry(350+sections_w+10, sections_y + 10, sections_w, 100);
    spreg->show();
}

void View::clear() {
    delete code_win;
    delete _data;
    delete gpreg;
    delete spreg;
    delete this;
}


//  SERVICES

void LoadService::operator() (Controller &c, bool def) {

    try {
        QString filename = def ? lastFile : getFilename(c.par);
        if (!filename.size()) return;
        auto filestream = std::ifstream(filename.toStdString().c_str());
        c.initViewCPU(to_strings(filestream));

        lastFile = filename;
    }
    catch (std::logic_error &e) {
        QString str
            = c.errorField.text() + "Load error" + e.what() + "\n";
        c.errorField.setText(str);
    }
}

void BuildService::operator() (Controller &c) {

    if (!c.cpuState || !c.view) return;

    try {
        c.cpuState->load(c.view->getLines());
        c.update(NO_BPS);
    }
    catch (std::logic_error &e) {
        QString str
            = c.errorField.text() + "Compile error: " + e.what() + "\n";
        c.errorField.setText(str);
    }
}

void RunService::operator() (Controller &c) {

    if (!c.cpuState || !c.view) return;

    auto guiBreakpoints = c.view->bps();
    auto avl_bps = c.cpuState->getAvlBps();
    std::size_t curAvlNum = 0;
    my_std::Vec <bpNum> bps;

    //  3 4 5 6 7 8
    //  0 1 2 3 4 5

    for (std::size_t curGuiNum = 0, textNum = 0; curGuiNum < guiBreakpoints.size(); curGuiNum++) {
        if (curAvlNum < avl_bps.size()) {
            guiBreakpoints.at(curGuiNum)->is_set
                    && curGuiNum == avl_bps.at(curAvlNum) ?
                bps.push_back(bpNum(textNum++, curGuiNum))
                      : guiBreakpoints.at(curGuiNum)->clear();

            if (curGuiNum >= avl_bps.at(curAvlNum)) curAvlNum++;
        }
        else guiBreakpoints.at(curGuiNum)->clear();

        guiBreakpoints.at(curGuiNum)->block();
    }

    auto dbg = [this, &c](bpNum bp_num)->void{update_view(bp_num, &c);};

    for (auto e : bps) qDebug() << e.textNum << ' '<< e.progNum << ' ';

    c.cpuState->execCPU(bps, dbg);

    for (std::size_t i = 0; i < guiBreakpoints.size(); i++) {
        guiBreakpoints[i]->unblock();
    }
}

void RunService::update_view(bpNum bp_num, Controller* c) {
    c->update(bp_num.progNum);
    c->waitLoop();
}


//  CONTROLLERS

void Controller::waitLoop() {
    QEventLoop loop;
    par.connect(&par, SIGNAL(resumeExec()), &loop, SLOT(quit()));
    loop.exec();
}

Controller::Controller(QWidget &_par) : par (_par),
   errorField (QLabel ("Current session:\n", const_cast<QWidget*>(&par))) {
   errorField.setAlignment(Qt::AlignLeft);
   errorField.setStyleSheet("background-color: #E0E0E0");
   errorField.setFixedSize(500, 200);
   errorField.move(10, par.parentWidget()->height()-errorField.height()-10);
   errorField.show();
};

UIController::UIController(QApplication &a, QWidget *par)
    : QWidget(par) {
    setGeometry(par->geometry());
    show();

    connect(this, SIGNAL(quit()), a.instance(), SLOT (quit()));

    QPushButton *b_quit = new QPushButton ("Exit", this);
    b_quit->setGeometry(width() - B_SZ - 10, 10, B_SZ, B_SZ);
    connect(b_quit, SIGNAL (clicked()), a.instance(), SLOT (quit()));
    b_quit->show();

    QPushButton *b_load = new QPushButton ("Load program", this);
    std::size_t load_b_sz = 3*B_SZ;
    b_load->setGeometry(b_quit->x() - load_b_sz - B_SZ - 10, b_quit->y(), load_b_sz, B_SZ);
    connect(b_load, SIGNAL (clicked()), this, SLOT (load()));
    b_load->show();

    QPushButton *b_run = new QPushButton ("Run", this);
    b_run->setGeometry(30, 10, B_SZ, B_SZ);
    b_run->setStyleSheet("background-color: #F72626");
    connect(b_run, SIGNAL (clicked()), this, SLOT (run()));
    b_run->show();

    QPushButton *b_build = new QPushButton ("Build", this);
    b_build->setGeometry(b_run->x() + B_SZ + 10, 10, B_SZ, B_SZ);
    b_build->setStyleSheet("background-color: #BD8F31");
    connect(b_build, SIGNAL (clicked()), this, SLOT (build()));
    b_build->show();

    QPushButton *b_resume = new QPushButton ("Resume", this);
    b_resume->setGeometry(b_build->x() + B_SZ + 10, 10, B_SZ, B_SZ);
    b_resume->setStyleSheet("background-color: #0066CC");
    connect(b_resume, SIGNAL (clicked()), this, SLOT (resume()));
    b_resume->show();

    loadS(Cntl, true);
}








