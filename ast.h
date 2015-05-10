#ifndef _AST_H_
#define _AST_H_

#include <string>
#include <list>

namespace ast
{

using namespace std;

class Base
{
public:
	Base();
};

class List
{
protected:
	Base *head;
	List *tail;

public:
	List(Base *_head, List *_tail = NULL);
	Base *get();
	List *next();
};

class Symbol:
	public Base
{
protected:
	size_t pos;
	string symbol;

public:
	Symbol(size_t _pos, string _symbol);
};

class IntNum:
	public Symbol
{
protected:
	int num;

public:
	IntNum(size_t _pos, string _symbol);
};

class FloatNum:
	public Symbol
{
protected:
	float num;

public:
	FloatNum(size_t _pos, string _symbol);
};

class Type:
	public Base
{
public:
	enum _Type {
		INT, FLOAT
	};

protected:
	_Type type;

public:
	Type(_Type _type);
};

class Identifier:
	public Base
{
protected:
	Symbol *name;

public:
	Identifier(Symbol *_name);
};

class IndexedIdentifier:
	public Identifier
{
protected:
	IntNum *index;

public:
	IndexedIdentifier(Symbol *_name, IntNum *_index);
};

class Declaration:
	public Base
{
protected:
	Type *type;
	list<Identifier *> identifier;

public:
	Declaration(Type *_type, List *_identifier);
};

class Parameter:
	public Base
{
protected:
	Type *type;
	Identifier *identifier;

public:
	Parameter(Type *_type, Identifier *_identifier);
};

class Expr:
	public Base
{
protected:
	Expr();
};

class UnOpExpr:
	public Expr
{
public:
	enum UnOp {
		MINUS
	};

protected:
	UnOp unOp;
	Expr *expr;

public:
	UnOpExpr(UnOp _unOp, Expr *_expr);
};

class BinOpExpr:
	public Expr
{
public:
	enum BinOp {
		PLUS, MINUS, MUL, DIV,
		EQ, NE, LT, LE, GT, GE
	};

protected:
	Expr *left;
	BinOp binOp;
	Expr *right;

public:
	BinOpExpr(Expr *_left, BinOp _binOp, Expr *_right);
};

class IntNumExpr:
	public Expr
{
protected:
	IntNum *intNum;

public:
	IntNumExpr(IntNum *_intNum);
};

class FloatNumExpr:
	public Expr
{
protected:
	FloatNum *floatNum;

public:
	FloatNumExpr(FloatNum *_floatNum);
};

class SymbolExpr:
	public Expr
{
protected:
	Symbol *name;

public:
	SymbolExpr(Symbol *_name);
};

class IndexedSymbolExpr:
	public SymbolExpr
{
protected:
	Expr *index;

public:
	IndexedSymbolExpr(Symbol *_name, Expr *_index);
};

class Stmt:
	public Base
{
public:
	Stmt();
};

class Call:
	public Stmt, Expr
{
protected:
	Symbol *name;
	list<Expr *> expr;

public:
	Call(Symbol *_name, List *_expr = NULL);
};

class CallStmt:
	public Stmt
{
protected:
	Call *call;

public:
	CallStmt(Call *_call);
};

class Assign:
	public Stmt
{
protected:
	Symbol *name;
	Expr *expr;

public:
	Assign(Symbol *_name, Expr *_expr);
};

class IndexedAssign:
	public Assign

{
protected:
	Expr *index;

public:
	IndexedAssign(Symbol *_name, Expr *_index, Expr *_expr);
};

class AssignStmt:
	public Stmt
{
protected:
	Assign *assign;

public:
	AssignStmt(Assign *_assign);
};

class RetStmt:
	public Stmt
{
protected:
	Expr *expr;

public:
	RetStmt(Expr *_expr = NULL);
};

class WhileStmt:
	public Stmt
{
protected:
	Expr *expr;
	Stmt *stmt;

public:
	WhileStmt(Expr *_expr, Stmt *_stmt);
};

class DoWhileStmt:
	public WhileStmt
{
public:
	DoWhileStmt(Expr *_expr, Stmt *_stmt);
};

class ForStmt:
	public Stmt
{
protected:
	Assign *first;
	Expr *second;
	Assign *third;
	Stmt *stmt;

public:
	ForStmt(Assign *_first, Expr *_second, Assign *_third, Stmt *_stmt);
};

class IfStmt:
	public Stmt
{
protected:
	Expr *expr;
	Stmt *than, *_else;

public:
	IfStmt(Expr *_expr, Stmt *_than, Stmt *__else = NULL);
};

class Case:
	public Base
{
protected:
	IntNum *index;
	list<Stmt *> stmt;
	bool _break;

public:
	Case(IntNum *_index, List *_stmt, bool __break);
};

class DefaultCase:
	public Case
{
public:
	DefaultCase(List *_stmt, bool __break);
};

class SwitchStmt:
	public Stmt
{
protected:
	Identifier *identifier;
	list<Case *> _case;

public:
	SwitchStmt(Identifier *_identifier, List *__case);
};

class CompoundStmt:
	public Stmt
{
protected:
	list<Declaration *> declaration;
	list<Stmt *> stmt;

public:
	CompoundStmt(List *_declaration, List *_stmt);
};


class Function:
	public Base
{
protected:
	Type *type;
	Symbol *name;
	list<Parameter *> parameter;
	CompoundStmt *compoundStmt;

public:
	Function(Type *_type, Symbol *_name, List *_parameter, CompoundStmt *_compoundStmt);
};

class Program:
	public Base
{
protected:
	list<Declaration *> declaration;
	list<Function *> function;

public:
	Program(List *_declaration, List *_function);
};

}

#endif
