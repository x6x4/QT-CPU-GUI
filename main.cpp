#include "mainwindow.h"

#include "/home/cracky/cppfun/3/lib/IR_compiler/fwd_IR_compiler.h"
#include "/home/cracky/cppfun/3/instr_set/instr_set.h"
#include <QApplication>
#include <QPushButton>
#include <QTextStream>
#include <QLabel>
#include <QFile>
#include <sstream>
//#include <QVector>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /*  GENERAL UI  */

    QWidget win;
    win.setFixedSize(600, 500);
    win.setToolTip("CPU emulator");

    QPushButton *b_main = new QPushButton (&win);
    b_main->setGeometry(10, 10, 500, 50);

    QPushButton *b_quit = new QPushButton ("Exit", &win);
    b_quit->setGeometry(520, 10, 50, 50);
    win.connect(b_quit, SIGNAL (clicked()), a.instance(), SLOT (quit())); 

    /*  DEFAULT SECTIONS  */

    std::size_t cell_h = 30;
    std::size_t cell_w = 30;
    std::size_t sections_y = 70;
    std::size_t header_w = 2*cell_w;

    //  DATA

    std::size_t data_x = 10;
    std::size_t data_h = cell_h;

    QLabel *data_header = new QLabel(&win);
    data_header->setFrameStyle(QFrame::StyledPanel);
    data_header->setGeometry(data_x, sections_y, header_w, data_h);
    data_header->setText("DATA");
    data_header->setStyleSheet("background-color: #8EDC7A");

    auto data = std::vector<QLabel*>(8);
    for (int i = 0; i < data.size(); i++) {
        data[i] = new QLabel(&win);
        data[i]->setFrameStyle(QFrame::StyledPanel);
        data[i]->setGeometry(data_x+header_w+i*cell_w, sections_y, cell_w, data_h);
    }

    //  GP REGS

    std::size_t gp_reg_x = 440;

    QLabel *gp_reg_header = new QLabel(&win);
    gp_reg_header->setFrameStyle(QFrame::StyledPanel);
    gp_reg_header->setGeometry(gp_reg_x, sections_y, header_w, cell_h);
    gp_reg_header->setText("GP REGS");
    gp_reg_header->setStyleSheet("background-color: #BDAEAE");

    auto gp_reg_block_frame = std::vector<QLabel*>(8 * 2);
    for (int i = 0; i < gp_reg_block_frame.size() - 1; i+=2) {
        gp_reg_block_frame[i] = new QLabel(&win);
        gp_reg_block_frame[i+1] = new QLabel(&win);
        gp_reg_block_frame[i]->setFrameStyle(QFrame::StyledPanel);
        gp_reg_block_frame[i+1]->setFrameStyle(QFrame::StyledPanel);
        gp_reg_block_frame[i]->setGeometry(gp_reg_x, sections_y+(i/2+1)*cell_h, cell_w, cell_h);
        gp_reg_block_frame[i+1]->setGeometry(gp_reg_x+cell_w, sections_y+(i/2+1)*cell_h, cell_w, cell_h);

        QVector<QString> reg_label{"r", QString::number(i/2)};
        const QString reg = std::accumulate(reg_label.cbegin(),reg_label.cend(), QString{});
        gp_reg_block_frame[i]->setText(reg);
    }

    //  SP REGS

    std::size_t sp_reg_x = gp_reg_x+header_w + 10;

    QLabel *sp_reg_header = new QLabel(&win);
    sp_reg_header->setFrameStyle(QFrame::StyledPanel);
    sp_reg_header->setGeometry(sp_reg_x, sections_y, header_w, cell_h);
    sp_reg_header->setText("SP REGS");
    sp_reg_header->setStyleSheet("background-color: #DAD59C");

    auto sp_reg_block_frame = std::vector<QLabel*>(2*2);
    for (int i = 0; i < 2*2; i+=2) {
        sp_reg_block_frame[i] = new QLabel(&win);
        sp_reg_block_frame[i+1] = new QLabel(&win);
        sp_reg_block_frame[i]->setFrameStyle(QFrame::StyledPanel);
        sp_reg_block_frame[i+1]->setFrameStyle(QFrame::StyledPanel);
        sp_reg_block_frame[i]->setGeometry(sp_reg_x, sections_y+(i/2+1)*cell_h, cell_w, cell_h);
        sp_reg_block_frame[i+1]->setGeometry(sp_reg_x+cell_w, sections_y+(i/2+1)*cell_h, cell_w, cell_h);
    }

    sp_reg_block_frame[0]->setText("pc");
    sp_reg_block_frame[2]->setText("zf");

    /* CPU-DEPENDENT PART  */

    CPU cpu(iset);
    std::ifstream is ("/home/cracky/cppfun/3/prog2.asm");
    strings vec = preproc (is);

    //  BREAKPOINTS

    std::size_t bp_x = data_x;
    std::size_t bp_y = sections_y+data_h+20;
    std::size_t button_w = 20;
    std::size_t button_h = 20;
    std::size_t bp_number = 8;
    auto breakpoints = std::vector<Breakpoint_Button*>(bp_number);
    for (int i = 0; i < breakpoints.size(); i++) {
        breakpoints[i] = new Breakpoint_Button (&win);
        breakpoints[i]->setText(QString::number(i+1));
        breakpoints[i]->setGeometry(bp_x, bp_y + i*button_h, button_w, button_h);
    }

    //  ASM CODE

    QLabel *asm_code = new QLabel(&win);
    asm_code->setFrameStyle(QFrame::StyledPanel);

    std::size_t code_x = bp_x+20;
    std::size_t code_w = 300;
    std::size_t line_h = 20;
    asm_code->setGeometry(code_x, bp_y, code_w, line_h*(bp_number));




    //cpu.load_mem(std::move(mcode));
    /*std::stringstream in;
    cpu.print_pm(in);
    QString prog_str = QString::fromStdString(in.str());

    asm_code->setText(prog_str);
    asm_code->setAlignment(Qt::AlignVCenter);*/

    //  RUN CPU

    Run_Button b_run (b_main, &cpu, &breakpoints);
    b_run.setStyleSheet("background-color: #F72626");
    b_run.setGeometry(10, 10, 50, 30);
    b_run.setText("Run");

    //cpu.exec_mcode(std::move(mcode));

    /*cpu.exec("/home/cracky/cppfun/3/prog2.asm");
    for (int i = 0; i < data.size(); i++) {
        data[i]->setText(QString::number(cpu.data(i)));
    }

    for (int i = 0; i < gp_reg_block_frame.size(); i+=2) {
        gp_reg_block_frame[i+1]->setText(QString::number(cpu.gp(i/2)));
    }
    sp_reg_block_frame[1]->setText(QString::number(cpu.sp(0)));
    sp_reg_block_frame[3]->setText(QString::number(cpu.sp(1)));*/

    win.show();
    return a.exec();
}
