#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
class CPU;

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

class Run_Button;

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

class Run_Button : public Click_Button {
    Q_OBJECT

private:
    CPU *cpu;
    std::vector<Breakpoint_Button*>* breakpoints;

public:
    Run_Button(QWidget *parent = nullptr, CPU *_cpu = nullptr,
               std::vector<Breakpoint_Button*>* bps = nullptr)
        : Click_Button (parent), cpu (_cpu), breakpoints (bps) {}

private slots:
    void on_click () override;
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
