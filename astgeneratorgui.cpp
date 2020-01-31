#include "astgeneratorgui.h"
#include "ui_astgeneratorgui.h"

ASTGeneratorGUI::ASTGeneratorGUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ASTGeneratorGUI)
{
    ui->setupUi(this);
}

ASTGeneratorGUI::~ASTGeneratorGUI()
{
    delete ui;
}

