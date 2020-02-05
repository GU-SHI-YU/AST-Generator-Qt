#ifndef ASTGENERATORGUI_H
#define ASTGENERATORGUI_H

#include <QMainWindow>
#include "token.h"
#include <QTreeWidget>
#include <QFileDialog>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class ASTGeneratorGUI; }
QT_END_NAMESPACE

class ASTGeneratorGUI : public QMainWindow
{
    Q_OBJECT

public:
    ASTGeneratorGUI(QWidget *parent = nullptr);
    ~ASTGeneratorGUI();
    void InitTree(AST t);
	void AddItem(QTreeWidgetItem* root, AST t);

private slots:
    void on_pushButton_clicked();

    void on_lineEdit_returnPressed();

    void on_pushButtonFile_clicked();

private:
    Ui::ASTGeneratorGUI *ui;
	QString trans(string s);
};
#endif // ASTGENERATORGUI_H
