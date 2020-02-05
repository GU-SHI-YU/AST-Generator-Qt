#define  _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "type_define.h"
#include  "function_decl.h"

void SaveToken(int type, string s)
{
	Token* p = (Token*)malloc(sizeof(Token));
	if (!p)
		exit(-1);
	p->type = type;
	p->data = s;
	p->next = last_token->next;
	last_token->next = p;
	last_token = last_token->next;
}

int CheckToken(int type, string s)
{
	Token* p = (Token*)malloc(sizeof(Token));
	if (!p)
		exit(-1);
	p = token_list->next;
	while (p)
	{
		if (p->type == type)
		{
			Node* q = p->data->next;
			Node* k= s->next;
			while (k && q)
			{
				if (k->data != q->data)
					break;
				k = k->next;
				q = q->next;
			}
			if (!k && !q)
			{
				last_token = p;
				return 1;
			}
		}
		p = p->next;
	}
	return 0;
}

int GetToken()
{
	Node* head = (Node*)malloc(sizeof(Node));
	if (!head)
		exit(-1);
	head->next = NULL;
	string token = head;
	char c;
	int flag = 0;
	while (true)
	{
		c = fgetc(fp);
		if (c == ' ' || c == '\r' || c == '\t')
			continue;
		if (c == '\n')
		{
			l_num++;
			continue;
		}
		break;
	}
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')||c=='_') //标识符和关键字
	{
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
		} while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')||(c>='0'&&c<='9')||c=='_'); //获取token
		ungetc(c, fp); //退回字符
		Node* p = head->next;
		int i, j;
		for (i = 0;i < KEY_WORDS_NUM;i++)
		{
			flag = 0;
			p = head->next;
			j = 0;
			while (p)
			{
				if (p->data != keywords[i][j++])
				{
					flag = 1;
					break;
				}
				p = p->next;
			}
			if (keywords[i][j] == '\0' && flag == 0)
			{
				return i + 2;
			} //是关键字
		}
		if(!CheckToken(ID,head))
			SaveToken(ID, head);
		return ID; //是标识符
	}
    if (c == '#')
    {
        c = fgetc(fp);
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
        } while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')||(c>='0'&&c<='9')||c=='_');
        ungetc(c, fp);
        Node* p = head->next;
        int i, j;
        for (i = 0;i < KEY_WORDS_NUM;i++)
        {
            flag = 0;
            p = head->next;
            j = 0;
            while (p)
            {
                if (p->data != keywords[i][j++])
                {
                    flag = 1;
                    break;
                }
                p = p->next;
            }
            if (keywords[i][j] == '\0' && flag == 0)
            {
                return i + 2;
            }
        }
    }
	if ((c >= '0' && c <= '9')) //常量
	{
		if (c == '0') //十六进制，八进制或者0开头的浮点数
		{
			Node* p = (Node*)malloc(sizeof(Node));
			if (!p)
				exit(-1);
			p->data = c;
			p->next = token->next;
			token->next = p;
			token = token->next;
			c = fgetc(fp);
			if (c == 'e' || c == 'E') //指数表示的浮点型
			{
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
				} while ((c >= '0' && c <= '9'));
				if (c == 'f' || c == 'F')
				{
					SaveToken(CONST_FLOAT, head);
					return CONST_FLOAT;
				}//单精度浮点型
				ungetc(c, fp);
				SaveToken(CONST_DOUBLE, head);
				return CONST_DOUBLE; //双精度浮点型
			}
			if (c == '.') //浮点数
			{
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
				} while ((c >= '0' && c <= '9'));
				if (c == 'e' || c == 'E') //指数表示浮点数
				{
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
					} while ((c >= '0' && c <= '9'));
					if (c == 'f' || c == 'F')
					{
						SaveToken(CONST_FLOAT, head);
						return CONST_FLOAT;
					}//单精度浮点型
					ungetc(c, fp);
					SaveToken(CONST_DOUBLE, head);
					return CONST_DOUBLE; //双精度浮点型
				}
				if (c == 'f' || c == 'F')
				{
					SaveToken(CONST_FLOAT, head);
					return CONST_FLOAT;
				}//单精度浮点型
				ungetc(c, fp);
				SaveToken(CONST_DOUBLE, head);
				return CONST_DOUBLE; //双精度浮点型
			}
			if (c == 'x') //十六进制
			{
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
				} while ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'));
				if (c == 'L' || c == 'l')
				{
					SaveToken(CONST_LONG, head);
					return CONST_LONG;
				}//长整型
				if ((c >= 'g' && c <= 'z') || (c >= 'G' && c <= 'Z'))
					return ERROR; //错误的十六进制
				ungetc(c, fp);
				SaveToken(CONST_INT, head);
				return CONST_INT; //整型
			}
			if (c >= '0' && c <= '7') //八进制
			{
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
				} while ((c >= '0' && c <= '7'));
				if (c == 'l' || c == 'L')
				{
					SaveToken(CONST_LONG, head);
					return CONST_LONG;
				}//长整型
				if (c >= '8' && c <= '9')
					return ERROR; //错误的八进制
				ungetc(c, fp);
				SaveToken(CONST_INT, head);
				return CONST_INT; //整型
			}
		} //0开头的情况处理完毕
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
		} while ((c >= '0' && c <= '9'));
		if (c == 'e' || c == 'E') //指数表示的浮点型
		{
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
			} while ((c >= '0' && c <= '9'));
			if (c == 'f' || c == 'F')
			{
				SaveToken(CONST_FLOAT, head);
				return CONST_FLOAT;
			}//单精度浮点型
			ungetc(c, fp);
			SaveToken(CONST_DOUBLE, head);
			return CONST_DOUBLE; //双精度浮点型
		}
		if (c == '.') //浮点型
		{
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
			} while ((c >= '0' && c <= '9'));
			if (c == 'e' || c == 'E') //指数表示的浮点型
			{
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
				} while ((c >= '0' && c <= '9'));
				if (c == 'f' || c == 'F')
				{
					SaveToken(CONST_FLOAT, head);
					return CONST_FLOAT;
				}//单精度浮点型
				ungetc(c, fp);
				SaveToken(CONST_DOUBLE, head);
				return CONST_DOUBLE; //双精度浮点型
			}
			if (c == 'f' || c == 'F')
			{
				SaveToken(CONST_FLOAT, head);
				return CONST_FLOAT;
			}//单精度浮点型
			ungetc(c, fp);
			SaveToken(CONST_DOUBLE, head);
			return CONST_DOUBLE; //双精度浮点型
		}
		if (c == 'L' || c == 'l')
		{
			SaveToken(CONST_LONG, head);
			return CONST_LONG;
		}//长整型
		ungetc(c, fp);
		SaveToken(CONST_INT, head);
		return CONST_INT; //整型
	}
	if(c == '#') //宏定义
	{
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
		} while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
		ungetc(c, fp);
		Node* p = head->next;
		int i, j;
		for (i = 14;i <= 15;i++)
		{
			flag = 0;
			p = head->next;
			j = 0;
			while (p)
			{
				if (p->data != keywords[i][j++])
				{
					flag = 1;
					break;
				}
				p = p->next;
			}
			if (keywords[i][j] == '\0' && flag == 0)
			{
				return i + 2;
			}//是include或define
		}
		return ERROR; //错误
	}
	if(c == '\"')
	{
		c = fgetc(fp);
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
		} while (c != '\"');
		Node* p = (Node*)malloc(sizeof(Node));
		if (!p)
			exit(-1);
		p->data = '\0';
		p->next = token->next;
		token->next = p;
		token = token->next;
		SaveToken(CONST_STRING, head);
		return CONST_STRING;
	}
	switch(c) //标点和运算符
	{
	case '!':
		if ((c = fgetc(fp)) == '=')
			return NE;
		ungetc(c, fp);
		return ERROR;
	case '+':
		return ADD;
	case '-':
		return MINUS;
	case '*':
		return MULTIPLY;
	case '/':
		if ((c = fgetc(fp)) == '/') //行注释
		{
			do
			{
				c = fgetc(fp);
			} while (c != '\n');
			return GetToken();
		}
		if (c == '*') //块注释
		{
			do
			{
				do
				{
					c = fgetc(fp);
				} while (c != '*');
				c = fgetc(fp);
			}
			while (c != '/');
			return GetToken();
		}
		ungetc(c, fp);
		return DIVIDE;
	case '%':
		return MOD;
	case '=':
		if ((c = fgetc(fp)) == '=')
			return EQ;
		ungetc(c, fp);
		return ASSIGN;
	case '>':
		if ((c = fgetc(fp)) == '=')
			return GE;
		ungetc(c, fp);
		return GT;
	case '<':
		if ((c = fgetc(fp)) == '=')
			return LE;
		ungetc(c, fp);
		return LS;
	case '&':
		if ((c = fgetc(fp)) == '&')
			return LAND;
		ungetc(c, fp);
		return ERROR;
	case '|':
		if ((c = fgetc(fp)) == '|')
			return LOR;
		ungetc(c, fp);
		return ERROR;
	case ',':
		return  COMMA;
	case ';':
		return SEMICOLON;
	case '(':
		return LL;
	case ')':
		return RL;
	case '[':
		return LM;
	case ']':
		return RM;
	case '{':
		return LG;
	case '}':
		return RG;
	default:
		if (feof(fp)) //读取到文件尾返回EOF
			return EOF;
		return ERROR;
	}
}
