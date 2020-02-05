#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

#include "get_token.cpp"
#include "astgeneratorgui.h"
#include "type_define.h"
#include "token.h"
#include "function_decl.h"

int Compare(int a, int b)
{
	if (a == LL)
		a = 40;
	if (a == RL)
		a = 41;
	if (a == 'e')
		a = 42;
	if (b == LL)
		b = 40;
	if (b == RL)
		b = 41;
	if (b == 'e')
		b = 42;
	return op_prior[a - 26][b - 26];
}

int Match(int type)
{
	if (word != type)
		return 0;
	word = GetToken();
	return 1;
}

AST MacroList()
{
	AST root = (ASTNode*)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->type = MACROLIST;
	if ((root->child = Macro()))
	{
		if ((root->child->brother = MacroList()))
			root->child->brother->brother = NULL;
	}
	else
		return NULL;
	return root;
}

AST Macro()
{
	AST root = (ASTNode*)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	Node* head = (Node*)malloc(sizeof(Node));
	if (!head)
		exit(-1);
	head->next = NULL;
	string token = head;
	if (Match(INCLUDE))
	{
		root->type = INCLUDEMACRO;
		if (word == CONST_STRING)
		{
			root->data.id_name = last_token->data;
			root->child = NULL;
		}
		else if (word == LS)
		{
			char c = fgetc(fp);
			do
			{
				Node* p = (Node*)malloc(sizeof(Node));
				if (!p)
					exit(-1);
				p->data = c;
				p->next = token->next;
				token->next = p;
				token = token->next;
				c = fgetc(fp);
				if (c == '\n')
					l_num++;
			} while (c != '>');
			root->data.id_name = head;
			root->child = NULL;
		}
		else
		{
			printf("%d:错误的文件包含命令！\n", l_num);
			return NULL;
		}
	}
	else if (word == DEFINE)
	{
		root->type = DEFINEMACRO;
		char c = fgetc(fp);
		while (c == ' ' || c == '\t' || c == '\r' || c == '\n')
		{
			c = fgetc(fp);
			if (c == '\n')
				l_num++;
		}
		do
		{
			Node* p = (Node*)malloc(sizeof(Node));
			if (!p)
				exit(-1);
			p->data = c;
			p->next = token->next;
			token->next = p;
			token = token->next;
			c = fgetc(fp);;
			if (c == '\n')
				l_num++;
		} while (c != ' ' && c != '\t' && c != '\r' && c != '\n');
		root->child = (ASTNode*)malloc(sizeof(ASTNode));
		if (!root->child)
			exit(-1);
		root->child->data.id_name = head;
		root->child->child = NULL;
		Node* head2 = (Node*)malloc(sizeof(Node));
		if (!head2)
			exit(-1);
		head2->next = NULL;
		string token2 = head2;
		while (c == ' ' || c == '\t' || c == '\r' || c == '\n')
		{
			c = fgetc(fp);
			if (c == '\n')
				l_num++;
		}
		do
		{
			Node* p = (Node*)malloc(sizeof(Node));
			if (!p)
				exit(-1);
			p->data = c;
			p->next = token2->next;
			token2->next = p;
			token2 = token2->next;
			c = fgetc(fp);;
			if (c == '\n')
				l_num++;
		} while (c != ' ' && c != '\t' && c != '\r' && c != '\n');
		ungetc(c, fp);
		root->child->brother = (ASTNode*)malloc(sizeof(ASTNode));
		if (!root->child->brother)
			exit(-1);
		root->child->brother->data.id_name = head2;
		root->child->brother->child = NULL;
	}
	else
		return NULL;
	word = GetToken();
	return root;
}

void Program(ASTGeneratorGUI *w)
{
	AST ast = (ASTNode*)malloc(sizeof(ASTNode));
	if (!ast)
		exit(-1);
	ast->type = PROGRAMME;
    word = GetToken();
	if ((ast->child = MacroList()))
	{
		if((ast->child->brother = ExternalDeclList()))
		{
			ast->child->brother->brother = NULL;
			ShowAST(ast, w);
		}
		else
			printf("测试文档错误！\n");
	}
	else if((ast->child = ExternalDeclList()))
	{
		ast->child->brother = NULL;
		ShowAST(ast,w);
	}
	else
		printf("测试文档错误！\n");
}

AST ExternalDeclList()
{
	if (word == EOF)
		return NULL;
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->brother = NULL;
	root->type = EXTERNALDECLLIST;
	if ((root->child = Decl()))
	{
		if((root->child->brother = ExternalDeclList()))
			root->child->brother->brother = NULL;
	}
	else
		return NULL;
	return root;
}

AST Decl()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->type = DECL;
	if ((root->child = DeclSpecs()))
	{
        if ((root->child->brother = InitDeclaratorList()))
			root->child->brother->brother = NULL;
		else
			return NULL;
	}
	else
	{
		printf("%d:错误的关键字！\n", l_num);
		return NULL;
	}
	if(root->child->brother->child->type!=FUNCTIONDECLARATOR)
	{
		if (Match(SEMICOLON));
		else
			return NULL;
	}
	return root;
}

AST DeclSpecs()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->type = DECLSPECS;
	if ((root->child = TypeDecl()))
	{
        if((root->child->brother = DeclSpecs()))
            root->child->brother->brother = NULL;
	}
	else
		return NULL;
	return root;
}

AST TypeDecl()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->type = TYPEDECL;
	if (word < VOID || word > UNSIGNED)
		return NULL;
	root->child = NULL;
	root->data.key_name = keywords[word - 2];
	word = GetToken();
	return root;
}

AST InitDeclaratorList()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->type = INITDECLARATIONLIST;
	if ((root->child = Declarator()))
	{
		if (Match(COMMA))
		{
			if ((root->child->brother = InitDeclaratorList()))
				root->child->brother->brother = NULL;;
		}
		else
		{
			root->child->brother = NULL;;
		}
	}
	else
		return NULL;
	return root;
}

AST Declarator()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	if ((root->child = Id()))
	{
		if(Match(LM))
		{
			root->type = ARRAYDECLARATOR;
			if ((root->child->brother = Const()))
			{
				if (Match(RM))
				{
					root->child->brother->brother = NULL;
				}
				else
					return NULL;
			}
			else 
				return NULL;
		}
		else if(Match(LL))
		{
			root->type = FUNCTIONDECLARATOR;
			if ((root->child->brother = FunctionDefinition()))
			{
				root->child->brother->brother = NULL;
			}
			else
				return NULL;
		}
		else
		{
			root->type = DECLARATOR;
			root->child->brother = NULL;
		}
	}
	else
	{
		printf("%d错误的标识符\n",l_num);
		return NULL;
	}
	return root;
}

AST ActualParamList()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->type = ACTUALPARAMLIST;
	if ((root->child = Exp()))
	{
		if (Match(COMMA))
		{
			root->child->brother = ActualParamList();
		}
	}
	else
		return NULL;
	return root;
}

AST FunctionDefinition()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->type = FUNCTIONDEFINITION;
	if ((root->child = FormalParamList()))
	{
		if (Match(RL))
		{
			if ((root->child->brother = StatBlock()))
				root->child->brother->brother = NULL;
		}
		else
			return NULL;
	}
	else
	{
		if (Match(RL))
		{
			if ((root->child = StatBlock()))
				root->child->brother = NULL;
		}
		else
			return NULL;
	}
	return root;
}

AST FormalParamList()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->type = FORMALPARAMLIST;
	if ((root->child = FormalParam()))
	{
		if (Match(COMMA))
		{
			if((root->child->brother = FormalParamList()))
				root->child->brother->brother = NULL;
		}
	}
	else
		return NULL;
	return root;
}

AST FormalParam()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->type = FORMALPARAM;
	if ((root->child = DeclSpecs()))
	{
		if ((root->child->brother = Id()))
		{
			root->child->brother->brother = NULL;
		}
		else
			return NULL;
	}
	else
		return NULL;
	return root;
}

AST StatBlock()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->type = STATBLOCK;
	if (Match(LG))
	{
		if ((root->child = LocalDeclList()))
		{
			root->child->brother = StatList();
			if (Match(RG));
			else
				return NULL;
		}
		else
		{
			if ((root->child = StatList()))
			{
				if (Match(RG))
					root->child->brother = NULL;
				else
					return NULL;
			}
			else
				return NULL;
		}
	}
	else
		return NULL;
	return root;
}

AST LocalDeclList()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->type = LOCALDECLLIST;
	if((root->child = LocalDecl()))
	{
		if(Match(COMMA))
		{
			root->child->brother = LocalDeclList();
		}
	}
	else
		return NULL;
	return root;
}

AST LocalDecl()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->type = LOCALDECL;
	if ((root->child = DeclSpecs()))
	{
		if ((root->child->brother = InitDeclaratorList()))
		{
			if (Match(SEMICOLON));
			else
				return NULL;
		}
		else
			return NULL;
	}
	else
		return NULL;
	return root;
}

AST StatList()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->type = STATLIST;
	if ((root->child = Stat()))
	{
		if ((root->child->brother = StatList()))
			root->child->brother->brother = NULL;
	}
	else
		return NULL;
	return root;
}

AST Stat()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->type = STAT;
	if ((root->child = ReturnStat()))
	{
		root->child->brother = NULL;
	}
	else if ((root->child = JumpStat()))
	{
		root->child->brother = NULL;
	}
	else if ((root->child = SelectStat()))
	{
		root->child->brother = NULL;
	}
	else if ((root->child = IterationStat()))
	{
		root->child->brother = NULL;
	}
	else if ((root->child = Exp()))
	{
		if (Match(SEMICOLON))
		{
			root->child->brother = NULL;
		}
		else
			return NULL;
	}
	else
		return NULL;
	return root;
}

AST ReturnStat()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->type = RETURNSTAT;
	if (Match(RETURN))
	{
		if ((root->child = Exp()))
		{
			if (Match(SEMICOLON))
				root->child->brother = NULL;
			else
				return NULL;
		}
		else
			return NULL;
	}
	else
		return NULL;
	return  root;
}

AST SelectStat()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->type = SELECTSTAT;
	if (Match(IF))
	{
		if (Match(LL))
		{
			if ((root->child = Exp()))
			{
				if (Match(RL))
				{
					if ((root->child->brother = StatBlock()))
					{
						if (Match(ELSE))
						{
							root->type = IFELSESTAT;
							if ((root->child->brother->brother = StatBlock()))
								root->child->brother->brother->brother = NULL;
							else if ((root->child->brother->brother = Stat()))
								root->child->brother->brother->brother = NULL;

						}
						else
						{
							root->type = IFSTAT;
							root->child->brother->brother = NULL;
						}
					}
					else if((root->child->brother = Stat()))
					{
						if (Match(ELSE))
						{
							root->type = IFELSESTAT;
							if ((root->child->brother->brother = StatBlock()))
								root->child->brother->brother->brother = NULL;
							else if ((root->child->brother->brother = Stat()))
								root->child->brother->brother->brother = NULL;
						}
						else
						{
							root->type = IFSTAT;
							root->child->brother->brother = NULL;
						}
					}
					else
						return NULL;
				}
				else
					return NULL;
			}
			else
				return NULL;
		}
		else
			return NULL;
	}
	else
		return NULL;
	return root;
}

AST JumpStat()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->type = JUMPSTAT;
	root->child = NULL;
	if (Match(BREAK))
	{
		if (Match(SEMICOLON))
			root->data.key_name = keywords[16];
		else
			return NULL;
	}
	else if (Match(CONTINUE))
	{
		if (Match(SEMICOLON))
			root->data.key_name = keywords[17];
		else
			return NULL;
	}
	else
		return NULL;
	return root;
}

AST IterationStat()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->type = ITERATIONSTAT;
	if(Match(WHILE))
	{
		root->type = WHILESTAT;
		if(Match(LL))
		{
			if((root->child = Exp()))
			{
				if (Match(RL))
				{
					if ((root->child->brother = StatBlock()))
					{
						root->data.key_name = keywords[12];
						root->child->brother->brother = NULL;
					}
					else if ((root->child->brother = Stat()))
					{
						root->data.key_name = keywords[12];
						root->child->brother->brother = NULL;
					}
					else
						return NULL;
				}
				else
					return NULL;
			}
			else
				return NULL;
		}
		else
			return NULL;
	}
	else if(Match(DO))
	{
		root->type = DOWHILESTAT;
		if ((root->child = StatBlock()))
		{
			if(Match(WHILE))
			{
				if(Match(LL))
				{
					if((root->child->brother = Exp()))
					{
						if (Match(RL))
						{
							if (Match(SEMICOLON))
							{
								root->child->brother->brother = NULL;
								root->data.key_name = keywords[13];
							}
							else
								return NULL;
						}
						else
							return NULL;
					}
					else
						return NULL;
				}
				else
					return NULL;
			}
			else
				return NULL;
		}
		else if ((root->child = Stat()))
		{
			if (Match(WHILE))
			{
				if (Match(LL))
				{
					if ((root->child->brother = Exp()))
					{
						if (Match(RL))
						{
							if (Match(SEMICOLON))
							{
								root->child->brother->brother = NULL;
								root->data.key_name = keywords[13];
							}
							else
								return NULL;
						}
						else
							return NULL;
					}
					else
						return NULL;
				}
				else
					return NULL;
			}
			else
				return NULL;
		}
		else
			return NULL;
	}
	else if(Match(FOR))
	{
		root->type = FORSTAT;
		ASTNode* p=root;
		if (Match(LL))
		{
			if ((p->child = Exp()))
			{
				p = p->child;
				if (Match(SEMICOLON))
				{
					if((p->brother = Exp()))
					{
						p = p->brother;
						if (Match(SEMICOLON))
						{
							if((p->brother = Exp()))
							{
								p = p->brother;
								if (Match(RL))
								{
									if ((p->brother = StatBlock()))
									{
										root->data.ci = 123;
										p->brother->brother = NULL;
									}
									else if ((p->brother = Stat()))
									{
										root->data.ci = 123;
										p->brother->brother = NULL;
									}
									else
										return NULL;
								}
								else
									return NULL;
							}
							else
							{
								if (Match(RL))
								{
									if ((p->brother = StatBlock()))
									{
										root->data.ci = 12;
										p->brother->brother = NULL;
									}
									else if ((p->brother = Stat()))
									{
										root->data.ci = 12;
										p->brother->brother = NULL;
									}
									else
										return NULL;
								}
								else
									return NULL;
							}
						}
						else
							return NULL;
					}
					else
					{
						if (Match(SEMICOLON))
						{
							if ((p->brother = Exp()))
							{
								p = p->brother;
								if (Match(RL))
								{
									if ((p->brother = StatBlock()))
									{
										root->data.ci = 13;
										p->brother->brother = NULL;
									}
									else if ((p->brother = Stat()))
									{
										root->data.ci = 13;
										p->brother->brother = NULL;
									}
									else
										return NULL;
								}
								else
									return NULL;
							}
							else
							{
								if (Match(RL))
								{
									if ((p->brother = StatBlock()))
									{
										root->data.ci = 1;
										p->brother->brother = NULL;
									}
									else if ((p->brother = Stat()))
									{
										root->data.ci = 1;
										p->brother->brother = NULL;
									}
									else
										return NULL;
								}
								else
									return NULL;
							}
						}
						else
							return NULL;
					}
				}
				else
					return NULL;
			}
			else
			{
				if (Match(SEMICOLON))
				{
					if ((p->child = Exp()))
					{
						p = p->child;
						if (Match(SEMICOLON))
						{
							if ((p->brother = Exp()))
							{
								p = p->brother;
								if (Match(RL))
								{
									if ((p->brother = StatBlock()))
									{
										root->data.ci = 23;
										p->brother->brother = NULL;
									}
									else if ((p->brother = Stat()))
									{
										root->data.ci = 23;
										p->brother->brother = NULL;
									}
									else
										return NULL;
								}
								else
									return NULL;
							}
							else
							{
								if (Match(RL))
								{
									if ((p->brother = StatBlock()))
									{
										root->data.ci = 2;
										p->brother->brother = NULL;
									}
									else if ((p->brother = Stat()))
									{
										root->data.ci = 2;
										p->brother->brother = NULL;
									}
									else
										return NULL;
								}
								else
									return NULL;
							}
						}
						else
							return NULL;
					}
					else
					{
						if (Match(SEMICOLON))
						{
							if ((p->child = Exp()))
							{
								p = p->child;
								if (Match(RL))
								{
									if ((p->brother = StatBlock()))
									{
										root->data.ci = 3;
										p->brother->brother = NULL;
									}
									else if ((p->brother = Stat()))
									{
										root->data.ci = 3;
										p->brother->brother = NULL;
									}
									else
										return NULL;
								}
								else
									return NULL;
							}
							else
							{
								if (Match(RL))
								{
									if ((p->child = StatBlock()))
									{
										root->data.ci = 0;
										p->child->brother = NULL;
									}
									else if ((p->child = Stat()))
									{
										root->data.ci = 0;
										p->child->brother = NULL;
									}
									else
										return NULL;
								}
								else
									return NULL;
							}
						}
						else
							return NULL;
					}
				}
				else
					return NULL;
			}
		}
		else
			return NULL;
	}
	else
		return NULL;
	return root;
}

AST Exp()
{
	ASTNode* p;
	ASTNode* ns = (ASTNode*)malloc(sizeof(ASTNode) * 20);
	if (!ns)
		exit(-1);
	int* ops = (int*)malloc(sizeof(int) * 20);
	if (!ops)
		exit(-1);
	int ns_top = 0;
	int ops_top = 0;
	ops[++ops_top] = 'e';
	int error = 0;
	char c = '\0';
	while (((word != 'e') || ops[ops_top] != 'e') && !error)
	{
		if ((p = Oprt()))
		{
			p->brother = NULL;
			ns[++ns_top] = *p;
		}
		else if (word == SEMICOLON)
		{
			c = ';';
			word = 'e';
		}
		else if ((word >= NE && word <= LAND) || word == LL || word == RL || word == 'e')
		{
			switch (Compare(ops[ops_top], word))
			{
			case '<':
				ops[++ops_top] = word;
				word = GetToken();
				break;
			case '=':
				if (ops_top == 0)
					error++;
				else
					ops[ops_top--];
				word = GetToken();
				break;
			case '>':
				ASTNode* t1;
				t1 = (ASTNode*)malloc(sizeof(ASTNode));
				ASTNode* t2;
				t2 = (ASTNode*)malloc(sizeof(ASTNode));
				int t;
				if (ns_top < 2)
					error++;
				else
				{
					*t1 = ns[ns_top--];
					*t2 = ns[ns_top--];
					ASTNode* n = (ASTNode*)malloc(sizeof(ASTNode));
					if (!n)
						exit(-1);
					t = ops[ops_top--];
					n->type = EXP;
					n->data.op = t;
					n->child = t1;
					n->child->brother = t2;
					n->brother = NULL;
					ns[++ns_top] = *n;
				}
				break;
			default:
				if (word == RL)
				{
					c = ')';
					word = 'e';
				}
				else
					error++;
			}
		}
		else
			error++;
	}
	if (c != '\0')
	{
		ungetc(c, fp);
		word = GetToken();
	}
	if (ns_top == 1 && ops[ops_top] == 'e' && !error)
	{
		AST root = (AST)malloc(sizeof(ASTNode));
		if (!root)
			exit(-1);
		*root = ns[ns_top];
		root->type = EXP;
		return root;
	}
	return NULL;
}

AST Oprt()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->type = OPRT;
	if ((root->child = Id()))
	{
		if (Match(LM))
		{
			if ((root->child->brother = Const()))
			{
				root->child->type = ARRAYNODE;
				if (Match(RM))
				{
					root->child->brother->brother = NULL;
				}
				else
					return NULL;
			}
			else
				return NULL;
		}
		else if (Match(LL))
		{
			if ((root->child->brother = Function()))
			{
				root->child->type = FUNCTIONNODE;
				root->child->brother->brother = NULL;
			}
			else
				return NULL;
		}
		else
		{
			root->child->brother = NULL;
		}
	}
	else if ((root->child = Const()))
	{
		root->child->brother = NULL;
	}
	else
		return NULL;
	return root;
}

AST Function()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	root->type = FUNCTIONNODE;
	if ((root->child = ActualParamList()))
	{
		if (Match(RL))
		{
			root->child->brother = NULL;
		}
		else
			return NULL;
	}
	else
	{
		if (Match(RL))
		{
			root->child = NULL;
		}
		else
			return NULL;
	}
	return root;
}

AST Id()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	if (word == ID)
	{
		root->type = IDNODE;
		root->data.id_name = last_token->data;
		root->child = NULL;
		word = GetToken();
	}
	else
		return NULL;
	return root;
}

AST Const()
{
	AST root = (AST)malloc(sizeof(ASTNode));
	if (!root)
		exit(-1);
	int i = 0;
	char temp[50];
	for (i = 0;i < 49;i++)
		temp[i] = '\0';
	Node* p = last_token->data->next;
	root->child = NULL;
	i = 0;
	while (p)
	{
		temp[i++] = p->data;
		p = p->next;
	}
	if (word == CONST_INT)
	{
		root->type = CONST_I;
		root->data.ci = atoi(temp);
	}
	else if (word == CONST_LONG)
	{
		root->type = CONST_L;
		root->data.cl = atol(temp);
	}
	else if (word == CONST_DOUBLE)
	{
		root->type = CONST_D;
		root->data.cd = atof(temp);
	}
	else if (word == CONST_FLOAT)
	{
		root->type = CONST_F;
		root->data.cf = atof(temp);
	}
	else if (word == CONST_STRING)
	{
		root->type = CONST_S;
		root->data.key_name = temp;
	}
	else
		return NULL;
	word = GetToken();
	return root;
}

void ShowAST(AST t, ASTGeneratorGUI* w)
{
    w->InitTree(t);
	//int flag = 0;
	//if (!t)
	//	return;
	//Node* p;
	//int j;
	//for (j = 0;j < i;j++)
	//	printf("\t");
	//switch (t->type)
	//{
	//case EXTERNALDECLLIST:
	//	printf("外部定义序列：\n");
	//	break;
	//case EXTERNALDECL:
	//	printf("外部定义：\n");
	//	break;
	//case DECL:
	//case DECLSPECS:
	//	goto next;
	//case TYPEDECL:
	//	printf("类型：%s\n",t->data.key_name);
	//	break;
	//case INITDECLARATIONLIST:
	//	printf("标识符序列：\n");
	//	break;
	//case DECLARATOR:
	//	printf("标识符：\n");
	//	break;
	//case ARRAYDECLARATOR:
	//	printf("数组标识符：\n");
	//	break;
	//case FUNCTIONDECLARATOR:
	//	printf("函数标识符：\n");
	//	break;
	//case ACTUALPARAMLIST:
	//	printf("实参列表：\n");
	//	break;
	//case FUNCTIONDEFINITION:
	//	printf("函数定义：\n");
	//	break;
	//case FORMALPARAMLIST:
	//	printf("形参列表：\n");
	//	break;
	//case FORMALPARAM:
	//	printf("形参：\n");
	//	break;
	//case STATBLOCK:
	//	printf("复合语句：\n");
	//	break;
	//case LOCALDECLLIST:
	//	printf("局部变量声明：\n");
	//	break;
	//case LOCALDECL:
	//	printf("局部变量：\n");
	//	break;
	//case STATLIST:
	//	goto next;
	//case STAT:
	//	printf("语句:\n");
	//	break;
	//case RETURNSTAT:
	//	printf("返回语句：\n");
	//	break;
	//case SELECTSTAT:
	//	goto next;
	//case IFELSESTAT:
	//	printf("if-else选择语句：\n");
	//	break;
	//case IFSTAT:
	//	printf("if选择语句：\n");
	//	break;
	//case JUMPSTAT:
	//	printf("跳转语句：%s\n",t->data.key_name);
	//	break;
	//case ITERATIONSTAT:
	//	printf("%s循环：\n", t->data.key_name);
	//	break;
	//case EXP:
	//	printf("表达式：\n操作符:%s\n",codes[t->data.op-26]);
	//	break;
	//case IDNODE:
	//	printf("ID:");
	//	p = t->data.id_name->next;
	//	while (p)
	//	{
	//		putchar(p->data);
	//		p = p->next;
	//	}
	//	putchar('\n');
	//	break;
	//case CONST_I:
	//	printf("整型常量：%d\n",t->data.ci);
	//	break;
	//case CONST_L:
	//	printf("长整型常量：%ld\n", t->data.cl);
	//	break;
	//case CONST_D:
	//	printf("双精度浮点型常量：%lf\n", t->data.cd);
	//	break;
	//case CONST_F:
	//	printf("单精度浮点整常量：%f\n", t->data.cf);
	//	break;
	//case CONST_S:
	//	printf("字符串常量：\n");
	//	p = t->data.id_name->next;
	//	while (p)
	//	{
	//		putchar(p->data);
	//		p = p->next;
	//	}
	//	putchar('\n');
	//	break;
	//default:
	//	goto next;
	//}
	//flag = 1;
	//next:
	//ShowAST(t->child, i+flag);
	//ShowAST(t->brother, i);
}
