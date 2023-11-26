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
#include <cstdlib>
#include <exception>
#include <fstream>
#include "/home/cracky/cppfun/3/instr_set/instr_set.h"
#include "qdebug.h"
#include "qmessagebox.h"
#include "qnamespace.h"
#include "mainwidgets.h"
#include "qwidget.h"



class Breakpoint_Button;
class Controller;
class LoadService;
class CPUState;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CPUState {
friend LoadService;

    CPU cpu = CPU(iset);
    strings lines;

    void load (QString filename, QWidget *par) {
        try {
            auto stream = std::ifstream(filename.toStdString().c_str());
            lines = to_strings(stream);
            cpu.load_mem(file_to_mcode(cpu.iSet(), lines));
        }
        catch (std::logic_error &e) {
            QMessageBox msg;
            msg.critical(par, "Load error", e.what());
            msg.setFixedSize(200, 50);
            throw;
        }
    }

    void load (QWidget *par) {
        try {
            cpu.load_mem(file_to_mcode(cpu.iSet(), lines));
        }
        catch (std::logic_error &e) {
            QMessageBox msg;
            msg.critical(par, "Load error", e.what());
            msg.setFixedSize(200, 50);
            throw;
        }
    }

public:
    const CPU& getCPU() const { return cpu; }
    const strings& getLines() const { return lines; }

};

class View {
friend CPUState;

    Code_Widget* code_win = nullptr;
    Data_Widget *_data = nullptr;
    GPREG_Widget *gpreg = nullptr;
    SPREG_Widget *spreg = nullptr;

    CPUState &cpuState;

public:

    View(QWidget *par, CPUState &_cpuState) : cpuState (_cpuState) {

        std::size_t sections_y = 70, sections_w = 100;

        code_win = new Code_Widget (par);
        code_win->move(LB_SZ, sections_y+B_SZ + 10);
        code_win->show();

        _data = new Data_Widget(par);
        _data->init(cpuState.getCPU());
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

    void update (const CPUState &cpu) {
        code_win->update(cpu.getLines());
        _data->update(cpu.getCPU());
        gpreg->update(cpu.getCPU());
        spreg->update(cpu.getCPU());
    }

    void clear() {
        delete code_win;
        delete _data;
        delete gpreg;
        delete spreg;
    }
};


class LoadService {
friend Controller;

    void operator() () {
        try {
            cpuState.load(getFilename(), main);
            view = new View (main, cpuState);
        }
        catch (std::exception &e) {
            qDebug() << e.what();
        }
    }

    QWidget *main = nullptr;
    View *view = nullptr;
    CPUState cpuState;

    LoadService (QWidget *par) : main (par) {}
    QString getFilename () {
        QString defPath = "/home/cracky/cppfun/3/programs";

        auto filename =
        QFileDialog().getOpenFileName(main,"Open", defPath, "Asm files (*.asm)");

        if (!filename.size()) {
            QMessageBox msg (main);
            msg.setFixedSize(200, 50);
            msg.critical(main, "Load error", "Critical file error");

            main->close();
        }

        return filename;
    }
};


class RunService {
friend Controller;

    void operator() () {  };

    RunService (QWidget *par) {

    }

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
    Q_OBJECT

    LoadService loadS = LoadService(this);
     RunService  runS = RunService(this);

private slots:
    void load () { loadS(); }
    void  run () { runS();  }

public:
    Controller (QApplication &a)
        : QWidget (new QWidget), main(a) {

        std::size_t win_w = 700;
        setFixedSize(win_w, 500);
        setToolTip("CPU emulator");
        parentWidget()->show();

        std::size_t b_sz = B_SZ;

        QPushButton *b_quit = new QPushButton ("Exit", this);
        b_quit->setGeometry(win_w - b_sz - 10, 10, b_sz, b_sz);
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
    }

private:
    QApplication &main;
    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->key() == Qt::Key_F5)
            run();
        else if (event->key() == Qt::Key_Escape)
            main.quit();
        else {};
    }
};


#endif // MAINWINDOW_H
