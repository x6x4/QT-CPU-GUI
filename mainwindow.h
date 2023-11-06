#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QThread>
#include <QGridLayout>
#include <QDialog>
#include <QLineEdit>
#include "/home/cracky/cppfun/3/lib/internals/cpu/cpu.h"
#include "qpushbutton.h"


class Run_Button;
class Breakpoint_Button;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; class Red_Button; }
QT_END_NAMESPACE

/*  OWN CLASSES  */

class Styled_Label : public QLabel {

Q_OBJECT

public:
    Styled_Label(QWidget *parent = nullptr);
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

class Code_Lines {

    std::vector<QLineEdit*> lines;

public:

    std::size_t size() { return lines.size(); }
    auto operator[] (std::size_t num) {
        return lines.at(num);
    }

    Code_Lines () {};

    Code_Lines (strings code_strings) {
        lines = std::vector<QLineEdit*>(code_strings.size());

        for (int i = 0; i < lines.size(); i++) {
            lines[i] = new QLineEdit(QString::fromStdString(code_strings[i]));
        }
    }
};

//  WIDGETS

class Code_Widget : public QWidget {
    Q_OBJECT

    Code_Lines code_lines;
    std::vector<Breakpoint_Button*> breakpoints;

public:
    Code_Widget (QWidget *parent, strings lines, std::size_t bp_sz);
    auto &bps () { return breakpoints; }
};

class Section_Widget : public QWidget {
    Q_OBJECT

protected:
    CPU *cpu;
    Cell_Block main_block;

public:
    Section_Widget (QWidget *parent, CPU *_cpu) : QWidget (parent), cpu (_cpu) {};
    virtual void update() = 0;
};

class Data_Widget : public Section_Widget {
    Q_OBJECT

public:
    Data_Widget (QWidget *parent, CPU *cpu);
    void update () override;
};

class GPREG_Widget : public Section_Widget {

    Q_OBJECT

public:
    GPREG_Widget (QWidget *parent, CPU *cpu);
    void update ();
};

class SPREG_Widget : public Section_Widget {

    Q_OBJECT

public:
    SPREG_Widget (QWidget *parent, CPU *cpu);
    void update ();
};

class CPU_Widgets {

    Code_Widget* code_win;
    Data_Widget *_data;
    GPREG_Widget *gpreg;
    SPREG_Widget *spreg;
    Run_Button *b_run;

public:
    CPU_Widgets (QWidget *parent, CPU *cpu, std::size_t b_sz, std::size_t little_b_sz);
};

//  BUTTONS

class Open_Button : public QPushButton {

Q_OBJECT

    std::size_t b_sz;
    std::size_t little_b_sz;
    CPU *cpu;
    CPU_Widgets *gui;

public:
    Open_Button (QWidget *parent, CPU *cpu, std::size_t _b_sz, std::size_t _little_b_sz);

private slots:
    void load ();
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
    Breakpoint_Button(QWidget *parent = nullptr, std::size_t bp_num = 0) : Click_Button (parent) {
        setText(QString::number(bp_num));
    }

private slots:
    void on_click () override;
};

class GUI_State {

friend Run_Button;

    std::vector<Section_Widget*> vec;

    CPU* cpu;
    Run_Button *run_button;

    void update();

public:
    void operator() ();
    GUI_State (std::vector<Section_Widget*> _vec, CPU *_cpu) : vec (_vec), cpu (_cpu) {}
};

class Run_Button : public Click_Button {
    Q_OBJECT

friend GUI_State;

private:
    std::vector<Breakpoint_Button*> breakpoints;
    GUI_State show_state;

public:
    Run_Button(QWidget *parent, std::vector<Breakpoint_Button*> bps, GUI_State _sections);

private slots:
    void on_click () override;
};


class RunDialog : public QDialog {
public:
    RunDialog(QPushButton* orig);
};


class CPU_Thread : public QThread
{
    Run_Button *run_button;

    void update ();

public:
    CPU_Thread(Run_Button *_rb) : run_button (_rb) {};

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
