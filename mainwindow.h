/**
 * @file mainwindow.h
 * @brief Contains main classes for the MVC interface model
 *        of the CPU emulator.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "/home/cracky/cppfun/3/instr_set/instr_set.h"
#include "mainwidgets.h"
#include "qevent.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class UIController;
class Controller;


class CPUState {

    CPU cpu = CPU(iset);
    my_std::Vec<std::size_t> avl_bps;

public:

    const auto &getCPU () const { return cpu; }
    const auto &getAvlBps () const { return avl_bps; }

    void load (strings v);
    void execCPU (my_std::Vec<bpNum>& bps,
               std::function<void(bpNum)> dbg) {
        exec(cpu, bps, dbg);
    }
};

class View {
friend Controller;

    Code_Widget* code_win = nullptr;
    Data_Widget *_data = nullptr;
    GPREG_Widget *gpreg = nullptr;
    SPREG_Widget *spreg = nullptr;

    View(QWidget *par, CPUState *_cpuState, strings lines);
    void clear();

public:
    auto getLines() { return code_win->getLines(); }
    const auto &bps()      { return code_win->bps(); }
};


//  Loads assembler source file to the screen

class LoadService {
friend UIController;

    void operator() (Controller &c, bool def);
};

//  Compiles assembler program

class BuildService {
friend UIController;

    void operator() (Controller &c);
};

//  Stops program on breakpoints and updates the view

class RunService {
friend UIController;

    void operator() (Controller &c);
    void update_view(bpNum bp_num, Controller *c);
};


class Controller {
friend LoadService;
friend BuildService;
friend RunService;

    View* view = nullptr;
    CPUState *cpuState = nullptr;
    QString lastFile = "/home/cracky/cppfun/3/programs/prog2.asm";

    void initViewCPU(strings lines);
    void update(size_t bp_num);
    void waitLoop();

public:

    QWidget &par;
    QLabel errorField;

    Controller(QWidget &_par);
    ~Controller() { delete view; delete cpuState; }
};


/**
 * @class UIController
 * @brief The main widget of emulator, handling various user actions.
 * Transfers control to Load, Build and Run services as needed.
 *
 * The quit button (or Esc) stops the emulation and exits the program.
 * The load button enables loading of assembler source text.
 * The build button enables compilation of assembler source.
 * The run button (or F5) starts the execution.
 * The resume button (or F10) resumes the execution.
 */
class UIController : public QWidget {

Q_OBJECT

private slots:
    void load()   { loadS(Cntl, false); }
    void build()  { buildS(Cntl); }
    void run()    { buildS(Cntl); runS(Cntl);  }
    void resume() { emit resumeExec(); }
signals:
    void resumeExec();
    void quit();

private:
    LoadService  loadS;
    BuildService buildS;
    RunService   runS;
    Controller Cntl = Controller(*this);

    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->key() == Qt::Key_F5)
            run();
        else if (event->key() == Qt::Key_Escape)
            emit quit();
        else if (event->key() == Qt::Key_F10) {
            emit resumeExec();
        }
    }

public:

    UIController (QApplication &a, QWidget *par);
};


#endif // MAINWINDOW_H
