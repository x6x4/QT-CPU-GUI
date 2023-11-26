#ifndef MAINWIDGETS_H
#define MAINWIDGETS_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include "qdebug.h"
#include "qobjectdefs.h"

#include "/home/cracky/cppfun/3/lib/IR_compiler/fwd_IR_compiler.h"
#include "qwidget.h"

#define B_SZ 50
#define LB_SZ 30

//  LABELS

class Styled_Label : public QLabel {

Q_OBJECT

public:
    Styled_Label(QWidget *parent = nullptr)
        : QLabel(parent) {
        setFrameStyle(QFrame::StyledPanel);
        setAlignment(Qt::AlignCenter);
    }
};

class Click_Button : public QPushButton {

Q_OBJECT

protected:
    bool is_unactive = 0;

public:

    Click_Button(QWidget *parent = nullptr)
    : QPushButton(parent) {
        is_unactive = 0;
        connect(this, &QPushButton::clicked, this, &Click_Button::on_click);
    }

private slots:
    void on_click () {
        if (is_unactive) return;
        before_freeze();
    }
    virtual void before_freeze () = 0;
};

class Breakpoint_Button : public Click_Button {

Q_OBJECT

public:
    Breakpoint_Button(QWidget *parent = nullptr, std::size_t bp_num = 0) : Click_Button (parent) {
        setFixedSize(B_SZ, B_SZ);
        setText(QString::number(bp_num));
    }

private slots:
    void before_freeze () override {

        if (Click_Button::is_unactive) {
            setStyleSheet("background-color: #FFFFFF");
            Click_Button::is_unactive = 0;
        }
        else {
            setStyleSheet("background-color: #F72626");
            Click_Button::is_unactive = 1;
        }
    };
};

class RunButton : public Click_Button {

Q_OBJECT

public:
    RunButton(QWidget *parent = nullptr) : Click_Button (parent) {
        setStyleSheet("background-color: #F72626");
    }

private slots:
    void before_freeze () override {

        if (!Click_Button::is_unactive) {
            Click_Button::is_unactive = 1;
        }
    };
};

//  WIDGETS

class Code_Lines {

    std::vector<QLineEdit*> lines;

public:

    std::size_t size() { return lines.size(); }
    auto at (std::size_t num) { return lines.at(num); }

    Code_Lines () {};

    Code_Lines (const strings &code_strings) {
        lines = std::vector<QLineEdit*>(code_strings.size());

        for (size_t i = 0; i < lines.size(); i++) {
            lines[i] = new QLineEdit(QString::fromStdString(code_strings[i].line));
            lines[i]->setFixedHeight(B_SZ);
            lines[i]->setFixedWidth(200);
        }
    }
};

class Code_Widget : public QWidget {
    Q_OBJECT

    Code_Lines code_lines;
    my_std::Vec<Breakpoint_Button*> breakpoints;

public:
    Code_Widget (QWidget *parent) : QWidget (parent) {};

    void update (const strings &lines) {
        code_lines = Code_Lines(lines);
        QGridLayout *cw_grid = new QGridLayout(this);

        for (size_t i = 0; i < code_lines.size(); i++) {
            Breakpoint_Button *bp = new Breakpoint_Button (this, i+1);
            breakpoints.push_back(bp);
            cw_grid->addWidget(bp, i,0);

            cw_grid->addWidget(code_lines.at(i), i,1, 1, -1);
        }

        cw_grid->deleteLater();

    }
    auto &bps () { return breakpoints; }
};

class Data_Widget : public QWidget {
    Q_OBJECT

    QGridLayout *data_grid = nullptr;

public:
    Data_Widget (QWidget *parent) : QWidget (parent) {

        Styled_Label *data_header = new Styled_Label(parent);
        data_header->setText("DATA");
        data_header->setStyleSheet("background-color: #8EDC7A");

        data_grid = new QGridLayout(this);
        data_grid->addWidget(data_header, 0,0, 1,2);
    }

    void init (const CPU &cpu) {
        for (size_t i = 0; i < cpu.data_sz(); i++) {
            auto lbl = new Styled_Label (this);
            data_grid->addWidget(lbl, 0, i+2);

            if (i < cpu.data_cap())
                lbl->setText(QString::number(cpu.data_cell(i)));
            else
                lbl->setText("0");
        }
    }

    void update (const CPU &cpu) {
        for (int i = 0; i < data_grid->columnCount(); i++) {
            auto lbl = static_cast<Styled_Label>(data_grid->itemAt(i)->widget());
            lbl.setText(QString::number(cpu.data_cell(i)));
        }
    };
};

class GPREG_Widget : public QWidget {
    Q_OBJECT

    QGridLayout *gp_grid = nullptr;

public:
    GPREG_Widget (QWidget *parent) : QWidget (parent) {

        Styled_Label *gp_header = new Styled_Label(parent);
        gp_header->setText("DATA");
        gp_header->setStyleSheet("background-color: #8EDC7A");

        gp_grid = new QGridLayout(this);
        gp_grid->addWidget(gp_header, 0,0, 1,2);
    }

    void init () {
        for (size_t i = 0; i < 8*2; i+=2) {
            auto name = new Styled_Label (this);
            name->setText(QString("r%1").arg(i/2));
            gp_grid->addWidget(name, i+1, 0);

            auto value = new Styled_Label (this);
            value->setText("0");
            gp_grid->addWidget(value, i+1, 1);
        }
    }

    void update (const CPU &cpu) {
        for (int i = 0; i < gp_grid->columnCount(); i+=2) {
            auto lbl = static_cast<Styled_Label>(gp_grid->itemAt(i)->widget());
            lbl.setText(QString::number(cpu.sp(i/2)));
        }
    };
};

class SPREG_Widget : public QWidget {
    Q_OBJECT

    QGridLayout *sp_grid = nullptr;

public:
    SPREG_Widget (QWidget *parent) : QWidget (parent) {

        Styled_Label *sp_header = new Styled_Label(parent);
        sp_header->setText("SP REGS");
        sp_header->setStyleSheet("background-color: #DAD59C");

        sp_grid = new QGridLayout(this);
        sp_grid->addWidget(sp_header, 0,0, 1,2);
    }

    void init () {
        for (size_t i = 0; i < 2*2; i+=2) {
            auto name = new Styled_Label (this);
            if (i == 0) name->setText("pc");
            if (i == 2) name->setText("zf");
            sp_grid->addWidget(name, i+1, 0);

            auto value = new Styled_Label (this);
            value->setText("0");
            sp_grid->addWidget(value, i+1, 1);
        }
    }

    void update (const CPU &cpu) {
        for (int i = 0; i < sp_grid->columnCount(); i+=2) {
            auto lbl = static_cast<Styled_Label>(sp_grid->itemAt(i)->widget());
            lbl.setText(QString::number(cpu.sp(i/2)));
        }
    };
};

#endif // MAINWIDGETS_H