#include "mainwindow.h"
#include "qapplication.h"
#include "qdebug.h"
#include "qeventloop.h"
#include "qwidget.h"
#include <QDialog>
#include <QDebug>
#include <QMessageBox>
#include <cstddef>
#include <functional>


void BuildService::operator() (Controller* c) {
    try {
        auto &cpuState = c->viewCntl->cpuState;
        my_std::Vec<std::size_t> avl_bps;
        auto mcode =
        file_to_mcode(cpuState->getCPU().iSet(),
                      cpuState->getLines(), avl_bps);

        cpuState->cpu.load_mem(std::move(mcode));

        c->viewCntl->avl_bps = avl_bps;
    }
    catch (std::logic_error &e) {
        QMessageBox msg;
        msg.critical(c, "Load error", e.what());
        msg.setFixedSize(200, 50);
        throw;
    }
}

void LoadService::operator() (Controller* c) {

    try {
        auto stream = std::ifstream(getFilename(c).toStdString().c_str());
        c->viewCntl->cpuState->lines = to_strings(stream);
        c->viewCntl->view = new View (c, c->viewCntl->cpuState);
    }
    catch (std::logic_error &e) {
        QMessageBox msg;
        msg.critical(c, "Load error", e.what());
        msg.setFixedSize(200, 50);
        throw;
    }
}

void RunService::operator() (Controller* c) {

    if (!c->viewCntl) return;

    auto guiBreakpoints = c->viewCntl->bps();
    auto avl_bps = c->viewCntl->avl_bps;
    std::size_t cur_cmd = 0;
    my_std::Vec <bpNum> bps;

    //  3 4 5 6 7 8
    //  0 1 2 3 4 5

    for (std::size_t i = 0, textNum = 0; i < guiBreakpoints.size(); i++) {
        if (cur_cmd < avl_bps.size()) {
            guiBreakpoints.at(i)->is_set && i == avl_bps.at(cur_cmd) ?
                bps.push_back(bpNum(textNum++, i)) : guiBreakpoints.at(i)->reset();

            if (i >= avl_bps.at(cur_cmd)) cur_cmd++;
        }
        else {
            guiBreakpoints.at(i)->reset();
        }

        guiBreakpoints.at(i)->block();
    }

    auto dbg = [this, c](bpNum bp_num)->void{update_view(bp_num, c);};

    for (auto e : bps) qDebug() << e.textNum << ' '<< e.progNum << ' ';

    exec(c->viewCntl->getCPU(), bps, dbg);

    for (std::size_t i = 0; i < guiBreakpoints.size(); i++) {
        guiBreakpoints[i]->unblock();
    }
}

void RunService::update_view(bpNum bp_num, Controller* c) {
    c->viewCntl->update(bp_num.progNum);
    c->makeCringe();
}









