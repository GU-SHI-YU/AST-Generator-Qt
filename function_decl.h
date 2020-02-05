#pragma once
#include "type_define.h"
#include "astgeneratorgui.h"

static void SaveToken(int type, string s);
static int CheckToken(int type, string s);
static int GetToken();

static int Compare(int a, int b);
static int Match(int type);
static void Program(ASTGeneratorGUI* w);
static AST MacroList();
static AST Macro();
static AST ExternalDeclList();
static AST Decl();
static AST DeclSpecs();
static AST InitDeclaratorList();
static AST TypeDecl();
static AST Declarator();
static AST ActualParamList();
static AST FunctionDefinition();
static AST FormalParamList();
static AST FormalParam();
static AST StatBlock();
static AST LocalDeclList();
static AST LocalDecl();
static AST StatList();
static AST Stat();
static AST ReturnStat();
static AST SelectStat();
static AST JumpStat();
static AST IterationStat();
static AST Exp();
static AST Oprt();
static AST Function();
static AST Const();
static AST Id();
static void ShowAST(AST t, ASTGeneratorGUI* w);
