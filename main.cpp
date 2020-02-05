#include "astgeneratorgui.h"
#include "parser.cpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    l_num = 1;
    QApplication a(argc, argv);
    ASTGeneratorGUI w;
    w.show();
    return a.exec();
}
