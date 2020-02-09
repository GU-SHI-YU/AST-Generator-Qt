#pragma once
#include <stdio.h>
#include <QMainWindow>

static FILE* fp;
static int l_num;
typedef struct Node {
	char data;
	Node* next;
}Node, * string;

typedef struct Token {
	int type;
	string data;
	Token* next;
}Token;

typedef struct ASTNode {
	int type;
	ASTNode* child;
	ASTNode* brother;
	union
	{
		char op;
		int ci;
		long cl;
		float cf;
		double cd;
		char* key_name;
		string id_name;
	}data;
}ASTNode, * AST;

static enum node_type
{
	PROGRAMME,
	MACROLIST,
	INCLUDEMACRO,
	DEFINEMACRO,
	EXTERNALDECLLIST,
	DECL,
	DECLSPECS,
	TYPEDECL,
	INITDECLARATIONLIST,
	DECLARATOR,
	ARRAYDECLARATOR,
	FUNCTIONDECLARATOR,
	ACTUALPARAMLIST,
	FUNCTIONDEFINITION,
	FORMALPARAMLIST,
	FORMALPARAM,
	STATBLOCK,
	LOCALDECLLIST,
	LOCALDECL,
	STATLIST,
	STAT,
	RETURNSTAT,
	SELECTSTAT,
	IFELSESTAT,
	IFSTAT,
	JUMPSTAT,
	ITERATIONSTAT,
	WHILESTAT,
	DOWHILESTAT,
	FORSTAT,
	EXP,
	OPRT,
	IDNODE,
	ARRAYNODE,
	FUNCTIONNODE,
	CONST_I,
	CONST_L,
	CONST_F,
	CONST_D,
	CONST_S
}node_type;

static int word;
