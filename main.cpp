#include "mainwindow.h"

#include "/home/cracky/cppfun/3/lib/IR_compiler/fwd_IR_compiler.h"
#include "/home/cracky/cppfun/3/instr_set/instr_set.h"
#include <QApplication>
#include <QPushButton>
#include <QTextStream>
#include <QLabel>
#include <QFile>
#include <sstream>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /*/  GENERAL NON-CPU UI  /*/

    QWidget win;
    win.setFixedSize(600, 500);
    win.setToolTip("CPU emulator");

    QPushButton *b_main = new QPushButton (&win);
    b_main->setGeometry(10, 10, 500, 50);

    QPushButton *b_quit = new QPushButton ("Exit", &win);
    b_quit->setGeometry(520, 10, 50, 50);
    win.connect(b_quit, SIGNAL (clicked()), a.instance(), SLOT (quit())); 

    /*/  GEOMETRY PART  /*/

    CPU cpu(iset);
    std::ifstream is ("/home/cracky/cppfun/3/prog2.asm");

    /*  DEFAULT SECTIONS  */

    std::size_t cell_h = 30;
    std::size_t cell_w = 30;
    std::size_t sections_y = 70;
    std::size_t header_w = 2*cell_w;
    std::size_t header_h = cell_h;

    //  BREAKPOINTS & CODE WINDOW

    strings lines = preproc_text(is);

    std::size_t bp_x = 10;
    std::size_t bp_y = sections_y+header_h+20;
    std::size_t button_w = 20;
    std::size_t button_h = 20;
    std::size_t line_w = 300;
    std::size_t num_ln = lines.size();

    auto breakpoints = std::vector<Breakpoint_Button*>(num_ln);
    auto code_lines = Cell_Block(num_ln, &win);
    for (int i = 0; i < num_ln; i++) {
        breakpoints[i] = new Breakpoint_Button (&win);
        breakpoints[i]->setText(QString::number(i+1));
        code_lines[i]->setText(QString::fromStdString(lines[i]));
        breakpoints[i]->setGeometry(bp_x, bp_y + i*button_h, button_w, button_h);
        code_lines[i]->setGeometry(bp_x+button_w, bp_y+i*button_h, line_w, button_h);
    }

    //  DATA

    std::unordered_set<ID> data_label_table;
    std::unique_ptr<Data> data = parse_data(is, data_label_table);

    std::size_t data_x = bp_x;
    std::size_t data_y = sections_y;

    Styled_Label *data_header = new Styled_Label(&win);
    data_header->setGeometry(data_x, data_y, header_w, header_h);
    data_header->setText("DATA");
    data_header->setStyleSheet("background-color: #8EDC7A");

    auto data_cells = Cell_Block(cpu.data_sz(), &win);
    for (int i = 0; i < data_cells.size(); i++) {
        data_cells[i]->setGeometry(data_x+header_w+i*cell_w, data_y, cell_w, cell_h);

        if (data && i < data->size())
            data_cells[i]->setText(QString::number((*data).at(i)));
        else
            data_cells[i]->setText("0");
    }

    //  GP REGS

    std::size_t gp_reg_x = 440;

    Styled_Label *gp_reg_header = new Styled_Label(&win);
    gp_reg_header->setGeometry(gp_reg_x, sections_y, header_w, header_h);
    gp_reg_header->setText("GP REGS");
    gp_reg_header->setStyleSheet("background-color: #BDAEAE");

    auto gp_reg_cells = Cell_Block(8*2, &win);
    for (int i = 0; i < gp_reg_cells.size() - 1; i+=2) {
        gp_reg_cells[i]->setGeometry(gp_reg_x, sections_y+(i/2+1)*cell_h, cell_w, cell_h);
        gp_reg_cells[i+1]->setGeometry(gp_reg_x+cell_w, sections_y+(i/2+1)*cell_h, cell_w, cell_h);

        QVector<QString> reg_label{"r", QString::number(i/2)};
        const QString reg = std::accumulate(reg_label.cbegin(),reg_label.cend(), QString{});
        gp_reg_cells[i]->setText(reg);

        gp_reg_cells[i+1]->setText("0");
    }

    //  SP REGS

    std::size_t sp_reg_x = gp_reg_x+header_w + 10;

    Styled_Label *sp_reg_header = new Styled_Label(&win);
    sp_reg_header->setGeometry(sp_reg_x, sections_y, header_w, header_h);
    sp_reg_header->setText("SP REGS");
    sp_reg_header->setStyleSheet("background-color: #DAD59C");

    auto sp_reg_cells = Cell_Block(2*2, &win);
    for (int i = 0; i < 2*2; i+=2) {
        sp_reg_cells[i]->setGeometry(sp_reg_x, sections_y+(i/2+1)*cell_h, cell_w, cell_h);
        sp_reg_cells[i+1]->setGeometry(sp_reg_x+cell_w, sections_y+(i/2+1)*cell_h, cell_w, cell_h);
    }

    sp_reg_cells[0]->setText("pc");
    sp_reg_cells[1]->setText("0");
    sp_reg_cells[2]->setText("zf");
    sp_reg_cells[3]->setText("0");

    /*/  LOGIC PART  /*/

    //  LOAD CPU

    std::unique_ptr<SafeText> text = parse_text(iset, lines, data_label_table);
    Mem mcode = std::make_pair(std::move(data), std::move(text));
    cpu.load_mem(std::move(mcode));

    //  RUN CPU

    std::vector<Cell_Block> vec = {data_cells, gp_reg_cells, sp_reg_cells};

    GUI_State gui_state (vec, &cpu);

    Run_Button b_run (b_main, &breakpoints, &gui_state);
    b_run.setStyleSheet("background-color: #F72626");
    b_run.setGeometry(10, 10, 50, 30);
    b_run.setText("Run");

    //  END

    win.show();
    return a.exec();
}
