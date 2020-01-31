#ifndef ASTGENERATORGUI_H
#define ASTGENERATORGUI_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class ASTGeneratorGUI; }
QT_END_NAMESPACE

class ASTGeneratorGUI : public QMainWindow
{
    Q_OBJECT

public:
    ASTGeneratorGUI(QWidget *parent = nullptr);
    ~ASTGeneratorGUI();

private:
    Ui::ASTGeneratorGUI *ui;
};
#endif // ASTGENERATORGUI_H
