#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qeventloop.h"
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QThread>
class CPU;
class Run_Button;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; class Red_Button; }
QT_END_NAMESPACE

/*  OWN CLASSES  */

class Styled_Label : public QLabel {

Q_OBJECT

public:
    Styled_Label(QWidget *parent = nullptr);
};

class Click_Button : public QPushButton {

Q_OBJECT

protected:
    bool is_clicked = 0;

public:
    bool get_clicked () { return is_clicked; }
    Click_Button(QWidget *parent = nullptr);

private slots:
    virtual void on_click () = 0;
};


class Breakpoint_Button : public Click_Button {

Q_OBJECT

friend Run_Button;

protected:
    bool is_frozen = 0;

public:
    bool get_frozen () { return is_frozen; }

public:
    Breakpoint_Button(QWidget *parent = nullptr) : Click_Button (parent) {}

private slots:
    void on_click () override;
};

class Cell_Block {

public:
    std::vector<Styled_Label*> cells;

    Cell_Block() {};

    Cell_Block(std::size_t num_cells, QWidget *ptr) {

        cells = std::vector<Styled_Label*>(num_cells);

        for (int i = 0; i < cells.size(); i++)
            cells[i] = new Styled_Label (ptr);
    }

    auto operator[] (std::size_t num) {
        return cells.at(num);
    }

    std::size_t size() {
        return cells.size();
    }
};

class GUI_State {

public:

    Cell_Block data_block;
    Cell_Block gpreg_block;
    Cell_Block spreg_block;

    CPU* cpu;
    Run_Button *run_button;

    GUI_State (std::vector<Cell_Block> &vec, CPU *_cpu) {
        cpu = _cpu;
        data_block = vec.at(0);
        gpreg_block = vec.at(1);
        spreg_block = vec.at(2);
    }

    void operator() ();
};

class Run_Button : public Click_Button {
    Q_OBJECT

friend GUI_State;

private:
    std::vector<Breakpoint_Button*>* breakpoints;
    GUI_State* state;

public:
    Run_Button(QWidget *parent = nullptr, std::vector<Breakpoint_Button*>* bps = nullptr,
               GUI_State *_sections = nullptr)
        : Click_Button(parent), breakpoints(bps), state(_sections) {}

private slots:
    void on_click () override;
};


class MyThread : public QThread
{
    Run_Button *run_button;

public:
    MyThread(Run_Button *_rb) : run_button (_rb) {};

    void run() override;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
