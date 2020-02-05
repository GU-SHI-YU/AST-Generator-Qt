#pragma once
#include "type_define.h"
#define KEY_WORDS_NUM 18
static enum token_type {
	ERROR, //错误
	ID, //标识符
	VOID, INT, CHAR, LONG, FLOAT, DOUBLE, SIGNED, UNSIGNED, //类型关键字
	IF, ELSE, FOR, WHILE, DO, //关键字 
	INCLUDE, DEFINE, //编译预处理
	BREAK, CONTINUE, RETURN, //跳转关键字
	CONST_INT, CONST_FLOAT, CONST_LONG, CONST_DOUBLE, CONST_STRING, //常量
	NE, MINUS, ADD, MULTIPLY, DIVIDE, MOD, EQ, ASSIGN, GE, GT, LE, LS, LOR, LAND, //运算符
	COMMA, SEMICOLON, LL, RL, LM, RM, LG, RG, LCMT, BBCMT, EBCMT //标点
}token_type;
static char keywords[KEY_WORDS_NUM][10] = { "void\0","int\0","char\0","long\0","float\0","double\0","signed\0","unsigned\0","if\0","else\0","for\0","while\0","do\0","include\0","define\0","break\0","continue\0","return\0" };
static char codes[25][5] = { "!=","-","+","*","/","%","==","=",">=",">","<=","<","||","&&",
					  ",",";","(",")","[","]","{","}","//","/*","*/" };
static char op_prior[17][18] = {
	"><<<<<>/<<<<>><>>",
	">>><<<>/>>>>>><>>",
	">>><<<>/>>>>>><>>",
	">>>>>>>/>>>>>><>>",
	">>>>>>>/>>>>>><>>",
	">>>>>>>/>>>>>><>>",
	"><<<<<>/<<<<>><>>",
	"<<<<<<<<<<<<<<<<>",
	"<<<<<<</>>>>>><>>",
	"<<<<<<</>>>>>><>>",
	"<<<<<<</>>>>>><>>",
	"<<<<<<</>>>>>><>>",
	"<<<<<<</<<<<>><>>",
	"<<<<<<</<<<<<><>>",
	"><<<<<>/>>>><<<=>",
	">>>>>>>/>>>>>>=/>",
	"<<<<<<<<<<<<<<</=" };

static Token* token_list;
static Token* last_token;
