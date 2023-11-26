#include "mainwindow.h"
#include "qwidget.h"
#include <QDialog>

#include <QDebug>
#include <QMessageBox>
#include <cstddef>

#include <stdexcept>

/*  OWN CLASSES  */

/*void GUI_State::operator() () {
    update();
    //if (!cpu->is_over()) RunDialog(run_button).exec();
};

RunDialog::RunDialog(QPushButton* orig) : QDialog(orig->parentWidget()) {
    setWindowFlags(Qt::Widget);
    this->setGeometry(orig->geometry());

    QPushButton *copy = new QPushButton ("Continue", this);
    copy->setStyleSheet("background-color: #BD8F31");
    copy->setFixedSize(orig->size());

    connect(copy, &QPushButton::clicked, this, &QDialog::close);
}*/


//  BUTTONS

    //  get bps and froze them
/*    my_std::Vec <std::size_t> bps;
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
*/






