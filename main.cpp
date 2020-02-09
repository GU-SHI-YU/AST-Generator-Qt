#include "astgeneratorgui.h"
#include "parser.cpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ASTGeneratorGUI w;
    w.show();
    return a.exec();
}
