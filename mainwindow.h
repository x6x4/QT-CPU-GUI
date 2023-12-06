/**
 * @file mainwindow.h
 * @brief Contains main classes for the MVC interface model of the CPU emulator.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

#include <QGridLayout>
#include <QFileDialog>
#include <QApplication>
#include <QKeyEvent>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <fstream>
#include "/home/cracky/cppfun/3/instr_set/instr_set.h"
#include "qapplication.h"
#include "qdebug.h"
#include "qdialog.h"
#include "qmessagebox.h"
#include "qnamespace.h"
#include "mainwidgets.h"
#include "qobject.h"
#include "qwidget.h"



class Breakpoint_Button;
class Controller;
class ViewController;
class LoadService;
class BuildService;
class CPUState;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CPUState {
friend LoadService;
friend BuildService;

    CPU cpu = CPU(iset);
    strings lines;

public:

    const auto &getCPU () const { return cpu; }
    const auto &getLines () const { return lines; }
};

//  View controller

class View {
friend ViewController;

    Code_Widget* code_win = nullptr;
    Data_Widget *_data = nullptr;
    GPREG_Widget *gpreg = nullptr;
    SPREG_Widget *spreg = nullptr;

public:

    View(QWidget *par, CPUState *_cpuState) {

        std::size_t sections_y = 70, sections_w = 100;

        code_win = new Code_Widget (par);
        code_win->move(LB_SZ, sections_y+B_SZ + 10);
        code_win->init(_cpuState->getLines());
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

    void clear() {
        delete code_win;
        delete _data;
        delete gpreg;
        delete spreg;
    }
};

//  Loads assembler source file to CPU

class LoadService {
friend Controller;

    void operator() (Controller* c);

    QString getFilename (QWidget *main) {
        QString defPath = "/home/cracky/cppfun/3/programs";

        auto filename =
        QFileDialog().getOpenFileName(main,"Open", defPath, "Asm files (*.asm)");

        if (!filename.size()) {
            QMessageBox msg (main);
            msg.setFixedSize(200, 50);
            msg.critical(main, "Load error", "Critical file error");
        }

        return filename;
    }
};

//  Compiles assembler program

class BuildService {
friend Controller;

    void operator() (Controller* c);
};

//  Stops program on breakpoints and updates the view

class RunService {
friend Controller;

    void operator() (Controller* c);
    void update_view(bpNum bp_num, Controller* c);
};


class ViewController {
friend LoadService;
friend BuildService;
friend RunService;

    View* view = nullptr;
    CPUState *cpuState = new CPUState;
    my_std::Vec<std::size_t> avl_bps;

    void update (size_t bp_num) {
        view->code_win->update(bp_num);
        qDebug() << "ok1 " << bp_num << '\n';
        view->_data->update(cpuState->getCPU());
        qDebug() << "ok2 " << bp_num << '\n';
        view->gpreg->update(cpuState->getCPU());
        qDebug() << "ok3 " << bp_num << '\n';
        view->spreg->update(cpuState->getCPU());
        qDebug() << "ok4 " << bp_num << '\n';
    }

    auto bps () { return view->code_win->bps(); }
    const auto &getCPU () { return cpuState->getCPU(); }
};

/**
 * @class Controller
 * @brief The main widget of emulator, handling various user actions.
 * Transfers control to Load and Run services as needed.
 *
 * The quit button (or Esc) stops the emulation and exits the program.
 * The load button enables loading of assembler source files.
 * The run button (or F5) starts the emulation.
 */
class Controller : public QWidget {    
friend LoadService;
friend BuildService;
friend RunService;


    Q_OBJECT

    LoadService  loadS = LoadService();
    BuildService buildS = BuildService();
    RunService   runS = RunService();

private slots:
    void load ()  { loadS(this); }
    void build () { buildS(this); }
    void run ()   { buildS(this); runS(this);  }

public:

    Controller (QApplication &a, QWidget *par)
        : QWidget (par), main(a) {

        setGeometry(par->geometry());
        show();

        std::size_t b_sz = B_SZ;

        QPushButton *b_quit = new QPushButton ("Exit", this);
        b_quit->setGeometry(width() - b_sz - 10, 10, b_sz, b_sz);
        connect(b_quit, SIGNAL (clicked()), a.instance(), SLOT (quit()));
        b_quit->show();

        QPushButton *b_load = new QPushButton ("Load program", this);
        std::size_t load_b_sz = 3*b_sz;
        b_load->setGeometry(b_quit->x() - load_b_sz - b_sz - 10, b_quit->y(), load_b_sz, b_sz);
        connect(b_load, SIGNAL (clicked()), this, SLOT (load()));
        b_load->show();

        QPushButton *b_run = new QPushButton ("Run", this);
        b_run->setGeometry(30, 10, b_sz, b_sz);
        b_run->setStyleSheet("background-color: #F72626");
        connect(b_run, SIGNAL (clicked()), this, SLOT (run()));
        b_run->show();

        QPushButton *b_build = new QPushButton ("Build", this);
        b_build->setGeometry(b_run->x() + b_sz + 10, 10, b_sz, b_sz);
        b_build->setStyleSheet("background-color: #BD8F31");
        connect(b_build, SIGNAL (clicked()), this, SLOT (build()));
        b_build->show();
    }


private:
    ViewController *viewCntl = new ViewController();
    QApplication &main;
    bool idle = true;

    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->key() == Qt::Key_F5)
            run();
        else if (event->key() == Qt::Key_Escape)
            main.quit();
        else if (event->key() == Qt::Key_F10) {
            emit resume();
        }
    }

    void makeCringe() {
        QEventLoop loop;
        connect(this, SIGNAL(resume()), &loop, SLOT(quit()));
        loop.exec();
        idle = true;
    }

signals:
    void resume();
};


#endif // MAINWINDOW_H
