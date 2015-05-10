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

	virtual string toString();
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
	virtual string toString();
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
	virtual string toString();
};

class Identifier:
	public Base
{
protected:
	Symbol *name;

public:
	Identifier(Symbol *_name);
	virtual string toString();
};

class IndexedIdentifier:
	public Identifier
{
protected:
	IntNum *index;

public:
	IndexedIdentifier(Symbol *_name, IntNum *_index);
	virtual string toString();
};

class Declaration:
	public Base
{
protected:
	Type *type;
	list<Identifier *> identifier;

public:
	Declaration(Type *_type, List *_identifier);
	virtual string toString();
};

class Parameter:
	public Base
{
protected:
	Type *type;
	Identifier *identifier;

public:
	Parameter(Type *_type, Identifier *_identifier);
	virtual string toString();
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
	virtual string toString();
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
	virtual string toString();
};

class IntNumExpr:
	public Expr
{
protected:
	IntNum *intNum;

public:
	IntNumExpr(IntNum *_intNum);
	virtual string toString();
};

class FloatNumExpr:
	public Expr
{
protected:
	FloatNum *floatNum;

public:
	FloatNumExpr(FloatNum *_floatNum);
	virtual string toString();
};

class SymbolExpr:
	public Expr
{
protected:
	Symbol *name;

public:
	SymbolExpr(Symbol *_name);
	virtual string toString();
};

class IndexedSymbolExpr:
	public SymbolExpr
{
protected:
	Expr *index;

public:
	IndexedSymbolExpr(Symbol *_name, Expr *_index);
	virtual string toString();
};

class Stmt:
	public Base
{
public:
	Stmt();
	virtual string toString();
};

class Call:
	public Stmt, Expr
{
protected:
	Symbol *name;
	list<Expr *> expr;

public:
	Call(Symbol *_name, List *_expr = NULL);
	virtual string toString();
};

class CallStmt:
	public Stmt
{
protected:
	Call *call;

public:
	CallStmt(Call *_call);
	virtual string toString();
};

class Assign:
	public Stmt
{
protected:
	Symbol *name;
	Expr *expr;

public:
	Assign(Symbol *_name, Expr *_expr);
	virtual string toString();
};

class IndexedAssign:
	public Assign

{
protected:
	Expr *index;

public:
	IndexedAssign(Symbol *_name, Expr *_index, Expr *_expr);
	virtual string toString();
};

class AssignStmt:
	public Stmt
{
protected:
	Assign *assign;

public:
	AssignStmt(Assign *_assign);
	virtual string toString();
};

class RetStmt:
	public Stmt
{
protected:
	Expr *expr;

public:
	RetStmt(Expr *_expr = NULL);
	virtual string toString();
};

class WhileStmt:
	public Stmt
{
protected:
	Expr *expr;
	Stmt *stmt;

public:
	WhileStmt(Expr *_expr, Stmt *_stmt);
	virtual string toString();
};

class DoWhileStmt:
	public WhileStmt
{
public:
	DoWhileStmt(Expr *_expr, Stmt *_stmt);
	virtual string toString();
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
	virtual string toString();
};

class IfStmt:
	public Stmt
{
protected:
	Expr *expr;
	Stmt *than, *_else;

public:
	IfStmt(Expr *_expr, Stmt *_than, Stmt *__else = NULL);
	virtual string toString();
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
	virtual string toString();
};

class DefaultCase:
	public Case
{
public:
	DefaultCase(List *_stmt, bool __break);
	virtual string toString();
};

class SwitchStmt:
	public Stmt
{
protected:
	Identifier *identifier;
	list<Case *> _case;

public:
	SwitchStmt(Identifier *_identifier, List *__case);
	virtual string toString();
};

class CompoundStmt:
	public Stmt
{
protected:
	list<Declaration *> declaration;
	list<Stmt *> stmt;

public:
	CompoundStmt(List *_declaration, List *_stmt);
	virtual string toString();
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
	virtual string toString();
};

class Program:
	public Base
{
protected:
	list<Declaration *> declaration;
	list<Function *> function;

public:
	static Program *program;

	Program(List *_declaration, List *_function);
	virtual string toString();
};

}

#endif
