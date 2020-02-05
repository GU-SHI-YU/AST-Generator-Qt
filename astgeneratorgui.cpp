#include "astgeneratorgui.h"
#include "parser.cpp"
#include "ui_astgeneratorgui.h"
#include "type_define.h"

ASTGeneratorGUI::ASTGeneratorGUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ASTGeneratorGUI)
{
	token_list = (Token*)malloc(sizeof(Token));
	if (!token_list)
		exit(-1);
	last_token = token_list;
	token_list->next = NULL;
    ui->treeWidget = new QTreeWidget;
    QStringList qsl;
    qsl << "类型" << "值";
    ui->treeWidget->setColumnCount(2);
    ui->treeWidget->setHeaderLabels(qsl);
    QHeaderView *head=ui->treeWidget->header();
    head->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->setupUi(this);
}

ASTGeneratorGUI::~ASTGeneratorGUI()
{
    delete ui;
}

void ASTGeneratorGUI::InitTree(AST t)
{
    QStringList qsl;
    qsl << "类型" << "值";
    ui->treeWidget->setColumnCount(2);
    ui->treeWidget->setHeaderLabels(qsl);
    QHeaderView *head=ui->treeWidget->header();
    head->setSectionResizeMode(QHeaderView::ResizeToContents);
    QTreeWidgetItem* root = new QTreeWidgetItem(ui->treeWidget, QStringList(QString("程序")));
    ui->treeWidget->addTopLevelItem(root);
	AddItem(root,t);
}

void ASTGeneratorGUI::AddItem(QTreeWidgetItem* root, AST t)
{
	QTreeWidgetItem* n;
	QStringList qsl;
	QString qs;
	AST p;
	switch (t->type)
	{
	case PROGRAMME:
		if(t->child->type == MACROLIST)
		{
			n = new QTreeWidgetItem(root, QStringList(QString("宏定义序列")));
			root->addChild(n);
			AddItem(n, t->child);
			n = new QTreeWidgetItem(root, QStringList(QString("外部定义序列")));
			root->addChild(n);
			AddItem(n, t->child->brother);
		}
		else
		{
			n = new QTreeWidgetItem(root, QStringList(QString("外部定义序列")));
			root->addChild(n);
			AddItem(n, t->child);
		}
		break;
	case MACROLIST:
		if(t->child->type == INCLUDEMACRO)
		{
			qsl << "文件包含" << trans(t->child->data.id_name);
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
		}
		else
		{
            qsl << "宏定义" << "将" +  trans(t->child->child->data.id_name) + "定义为" + trans(t->child->child->brother->data.id_name);
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
		}
		if (t->child->brother)
			AddItem(root, t->child->brother);
		break;
	case EXTERNALDECLLIST:
		n = new QTreeWidgetItem(root, QStringList(QString("声明")));
		root->addChild(n);
		AddItem(n, t->child);
		if (t->child->brother)
			AddItem(root, t->child->brother);
		break;
	case DECL:
	case LOCALDECL:
		p = t->child;
		qs += p->child->data.key_name;
		p = p->child->brother;
		while(p)
		{
			qs.append(' ');
			qs += p->child->data.key_name;
			p = p->child->brother;
		}
		qsl << "类型" << qs;
		n = new QTreeWidgetItem(root, qsl);
		root->addChild(n);
		AddItem(root, t->child->brother);
		break;
	case INITDECLARATIONLIST:
		if(t->child->type == DECLARATOR)
		{
			qsl << "变量名" << trans(t->child->child->data.id_name);
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
		}
		else if(t->child->type == ARRAYDECLARATOR)
		{
			qsl << "数组名" << trans(t->child->child->data.id_name);
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
		}
		else
		{
			qsl << "函数名" << trans(t->child->child->data.id_name);
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
			AddItem(n, t->child->child->brother);
		}
		if (t->child->brother)
			AddItem(root, t->child->brother);
		break;
	case FUNCTIONDEFINITION:
		if (t->child->type == FORMALPARAMLIST)
		{
			n = new QTreeWidgetItem(root, QStringList(QString("形参列表")));
			root->addChild(n);
			AddItem(n, t->child);
			n = new QTreeWidgetItem(root, QStringList(QString("函数体")));
			root->addChild(n);
			AddItem(n, t->child->brother);
		}
		else
		{
			n = new QTreeWidgetItem(root, QStringList(QString("函数体")));
			root->addChild(n);
			AddItem(n, t->child);
		}
		break;
	case FORMALPARAMLIST:
		n = new QTreeWidgetItem(root, QStringList(QString("形参")));
		root->addChild(n);
		AddItem(n, t->child);
		if (t->child->brother)
			AddItem(root, t->child->brother);
		break;
	case FORMALPARAM:
		p = t->child;
		qs += p->child->data.key_name;
		p = p->child->brother;
		while (p)
		{
			qs.append(' ');
			qs += p->child->data.key_name;
			p = p->child->brother;
		}
		qsl << "类型" << qs;
		n = new QTreeWidgetItem(root, qsl);
		root->addChild(n);
		qs = trans(t->child->brother->data.id_name);
		n = new QTreeWidgetItem(n, QStringList(qs));
		root->addChild(n);
		break;
	case STATBLOCK:
		AddItem(root, t->child);
		if (t->child->brother)
			AddItem(root, t->child->brother);
		break;
	case LOCALDECLLIST:
		n = new QTreeWidgetItem(root, QStringList(QString("局部声明")));
		root->addChild(n);
		AddItem(n, t->child);
		if (t->child->brother)
			AddItem(root, t->child->brother);
		break;
	case STATLIST:
		AddItem(root, t->child);
		if (t->child->brother)
			AddItem(root, t->child->brother);
        break;
	case STAT:
		if(t->child->type == EXP)
		{
			n = new QTreeWidgetItem(root, QStringList(QString("表达式语句")));
			root->addChild(n);
			AddItem(n, t->child);
		}
		else if(t->child->type == RETURNSTAT)
		{
			n = new QTreeWidgetItem(root, QStringList(QString("返回语句")));
			root->addChild(n);
			AddItem(n, t->child);
		}
		else if(t->child->type == IFSTAT)
		{
			n = new QTreeWidgetItem(root, QStringList(QString("if语句")));
			root->addChild(n);
			AddItem(n, t->child);
		}
		else if (t->child->type == IFELSESTAT)
		{
			n = new QTreeWidgetItem(root, QStringList(QString("if-else语句")));
			root->addChild(n);
			AddItem(n, t->child);
		}
		else if (t->child->type == JUMPSTAT)
		{
			qsl << "跳转语句" << t->child->data.key_name;
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
		}
		else if(t->child->type == WHILESTAT)
		{
			n = new QTreeWidgetItem(root, QStringList(QString("while语句")));
			root->addChild(n);
			AddItem(n, t->child);
		}
		else if (t->child->type == DOWHILESTAT)
		{
			n = new QTreeWidgetItem(root, QStringList(QString("do-while语句")));
			root->addChild(n);
			AddItem(n, t->child);
		}
		else if (t->child->type == FORSTAT)
		{
			n = new QTreeWidgetItem(root, QStringList(QString("for语句")));
			root->addChild(n);
			AddItem(n, t->child);
		}
		break;
	case RETURNSTAT:
		n = new QTreeWidgetItem(root, QStringList(QString("返回表达式")));
		root->addChild(n);
		AddItem(n, t->child);
		break;
	case IFSTAT:
		n = new QTreeWidgetItem(root, QStringList(QString("满足条件")));
		root->addChild(n);
		AddItem(n, t->child);
		n = new QTreeWidgetItem(root, QStringList(QString("执行")));
		root->addChild(n);
		AddItem(n, t->child->brother);
		break;
	case IFELSESTAT:
		n = new QTreeWidgetItem(root, QStringList(QString("满足条件")));
		root->addChild(n);
		AddItem(n, t->child);
		n = new QTreeWidgetItem(root, QStringList(QString("执行")));
		root->addChild(n);
		AddItem(n, t->child->brother);
		n = new QTreeWidgetItem(root, QStringList(QString("否则执行")));
		root->addChild(n);
		AddItem(n, t->child->brother->brother);
		break;
	case WHILESTAT:
		n = new QTreeWidgetItem(root, QStringList(QString("满足条件时")));
		root->addChild(n);
		AddItem(n, t->child);
		n = new QTreeWidgetItem(root, QStringList(QString("重复执行")));
		root->addChild(n);
		AddItem(n, t->child->brother);
		break;
	case DOWHILESTAT:
		n = new QTreeWidgetItem(root, QStringList(QString("重复执行")));
		root->addChild(n);
		AddItem(n, t->child);
		n = new QTreeWidgetItem(root, QStringList(QString("如果满足条件")));
		root->addChild(n);
		AddItem(n, t->child->brother);
		break;
	case FORSTAT:
		if(t->data.ci == 123)
		{
			n = new QTreeWidgetItem(root, QStringList(QString("单次表达式")));
			root->addChild(n);
			AddItem(n, t->child);
			n = new QTreeWidgetItem(root, QStringList(QString("满足条件")));
			root->addChild(n);
			AddItem(n, t->child->brother);
			n = new QTreeWidgetItem(root, QStringList(QString("末尾表达式")));
			root->addChild(n);
			AddItem(n, t->child->brother->brother);
			n = new QTreeWidgetItem(root, QStringList(QString("重复执行")));
			root->addChild(n);
			AddItem(n, t->child->brother->brother->brother);
		}
		else if(t->data.ci == 12)
		{
			n = new QTreeWidgetItem(root, QStringList(QString("单次表达式")));
			root->addChild(n);
			AddItem(n, t->child);
			n = new QTreeWidgetItem(root, QStringList(QString("满足条件")));
			root->addChild(n);
			AddItem(n, t->child->brother);
			n = new QTreeWidgetItem(root, QStringList(QString("重复执行")));
			root->addChild(n);
			AddItem(n, t->child->brother->brother);
		}
		else if(t->data.ci == 23)
		{
			n = new QTreeWidgetItem(root, QStringList(QString("满足条件")));
			root->addChild(n);
			AddItem(n, t->child);
			n = new QTreeWidgetItem(root, QStringList(QString("末尾表达式")));
			root->addChild(n);
			AddItem(n, t->child->brother);
			n = new QTreeWidgetItem(root, QStringList(QString("重复执行")));
			root->addChild(n);
			AddItem(n, t->child->brother->brother);
		}
		else if(t->data.ci == 13)
		{
			n = new QTreeWidgetItem(root, QStringList(QString("单次表达式")));
			root->addChild(n);
			AddItem(n, t->child);
			n = new QTreeWidgetItem(root, QStringList(QString("末尾表达式")));
			root->addChild(n);
			AddItem(n, t->child->brother);
			n = new QTreeWidgetItem(root, QStringList(QString("重复执行")));
			root->addChild(n);
			AddItem(n, t->child->brother->brother);
		}
		else if(t->data.ci == 1)
		{
			n = new QTreeWidgetItem(root, QStringList(QString("单次表达式")));
			root->addChild(n);
			AddItem(n, t->child);
			n = new QTreeWidgetItem(root, QStringList(QString("重复执行")));
			root->addChild(n);
			AddItem(n, t->child->brother);
		}
		else if(t->data.ci == 2)
		{
			n = new QTreeWidgetItem(root, QStringList(QString("条件表达式")));
			root->addChild(n);
			AddItem(n, t->child);
			n = new QTreeWidgetItem(root, QStringList(QString("重复执行")));
			root->addChild(n);
			AddItem(n, t->child->brother);
		}
		else if(t->data.ci == 3)
		{
			n = new QTreeWidgetItem(root, QStringList(QString("末尾表达式")));
			root->addChild(n);
			AddItem(n, t->child);
			n = new QTreeWidgetItem(root, QStringList(QString("重复执行")));
			root->addChild(n);
			AddItem(n, t->child->brother);
		}
		else if(t->data.ci == 0)
		{
			n = new QTreeWidgetItem(root, QStringList(QString("重复执行")));
			root->addChild(n);
			AddItem(n, t->child);
		}
		break;
	case EXP:
		if(t->child->type == OPRT || t->child->type == EXP)
		{
			qsl << "操作符" <<  codes[t->data.op - 25];
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
			AddItem(n, t->child->brother);
			AddItem(n, t->child);
		}
		else if(t->child->type == IDNODE)
		{
			qsl << "标识符" << trans(t->child->data.id_name);
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
		}
		else if(t->child->type == CONST_I)
		{
			qsl << "整型常量" << QString::number(t->child->data.ci);
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
		}
		else if (t->child->type == CONST_L)
		{
			qsl << "长整型常量" << QString::number(t->child->data.cl);
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
		}
		else if(t->child->type == CONST_D)
		{
			qsl << "双精度浮点型常量" << QString::number(t->child->data.cd);
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
		}
		else if (t->child->type == CONST_D)
		{
			qsl << "双精度浮点型常量" << QString::number(t->child->data.cd);
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
		}
		else if (t->child->type == FUNCTIONNODE)
		{
			qsl << "函数调用" << trans(t->child->data.id_name);
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
			AddItem(n, t->child->brother);
		}
		if (t->child->type == ARRAYNODE)
		{
			qsl << "数组元素" << trans(t->child->data.id_name) + "[" + QString::number(t->child->brother->data.ci) + "]";
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
		}
		break;
	case OPRT:
		if (t->child->type == IDNODE)
		{
			qsl << "标识符" << trans(t->child->data.id_name);
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
		}
		else if (t->child->type == CONST_I)
		{
			qsl << "整型常量" << QString::number(t->child->data.ci);
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
		}
		else if (t->child->type == CONST_L)
		{
			qsl << "长整型常量" << QString::number(t->child->data.cl);
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
		}
		else if (t->child->type == CONST_D)
		{
			qsl << "双精度浮点型常量" << QString::number(t->child->data.cd);
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
		}
		else if (t->child->type == CONST_D)
		{
			qsl << "双精度浮点型常量" << QString::number(t->child->data.cd);
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
		}
		else if (t->child->type == FUNCTIONNODE)
		{
			qsl << "函数调用" << trans(t->child->data.id_name);
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
			AddItem(n, t->child->brother);
		}
		if (t->child->type == ARRAYNODE)
		{
			qsl << "数组元素" << trans(t->child->data.id_name) + "[" + QString::number(t->child->brother->data.ci) + "]";
			n = new QTreeWidgetItem(root, qsl);
			root->addChild(n);
		}
		break;
	case FUNCTIONNODE:
		if (t->child)
		{
			n = new QTreeWidgetItem(root, QStringList(QString("实参列表")));
			root->addChild(n);
			AddItem(n, t->child);
		}
		else
		{
			n = new QTreeWidgetItem(root, QStringList(QString("无参数")));
			root->addChild(n);
		}
		break;
    default:
        break;
	}
}

QString ASTGeneratorGUI::trans(string s)
{
	QString qs = "";
	Node* p = s->next;
	while(p)
	{
		qs.append(p->data);
		p = p->next;
	}
	return qs;
}


void ASTGeneratorGUI::on_pushButton_clicked()
{
    ui->treeWidget->clear();
    QString s = ui->lineEdit->text();
    QByteArray ba = s.toLatin1();
    fp = fopen(ba.data(),"r");
    Program(this);
    fseek(fp,0,SEEK_SET);
    QFile qf;
    qf.open(fp,QIODevice::ReadWrite,QFileDevice::AutoCloseHandle);
    ui->textEdit->setPlainText(qf.readAll());
    qf.close();
}

void ASTGeneratorGUI::on_lineEdit_returnPressed()
{
   ASTGeneratorGUI::on_pushButton_clicked();
}

void ASTGeneratorGUI::on_pushButtonFile_clicked()
{
    ui->treeWidget->clear();
    QString s = QFileDialog::getOpenFileName(this, tr("打开文件"), tr("C:\\"), tr("C Files (*.cpp)"));
    QByteArray ba = s.toLatin1();
    fp = fopen(ba.data(),"r");
    Program(this);
    fseek(fp,0,SEEK_SET);
    QFile qf;
    qf.open(fp,QIODevice::ReadWrite,QFileDevice::AutoCloseHandle);
    ui->textEdit->setPlainText(qf.readAll());
    qf.close();
}
