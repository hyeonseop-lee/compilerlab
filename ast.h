#ifndef _AST_H_
#define _AST_H_

#include <string>
#include <list>
#include <map>
#include <functional>

namespace ast
{

class Scope;
class AbstractType;
class AbstractFunctionType;

using namespace std;

class Base
{
public:
	Base();

	virtual string toString();
	virtual void traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
};

class Pos
{
public:
	size_t pos;

	Pos(size_t _pos);
};

class List
{
public:
	Base *head;
	List *tail;

	List(Base *_head, List *_tail = NULL);
	Base *get();
	List *next();
};

class Symbol:
	public Base, public Pos
{
public:
	string symbol;

	Symbol(size_t _pos, string _symbol);
	virtual string toString();
};

class IntNum:
	public Symbol
{
public:
	int num;

	IntNum(size_t _pos, string _symbol);
};

class FloatNum:
	public Symbol
{
public:
	float num;

	FloatNum(size_t _pos, string _symbol);
};

class Type:
	public Base
{
public:
	enum _Type {
		INT, FLOAT
	};

	_Type type;

	Type(_Type _type);
	virtual string toString();
};

class Identifier:
	public Base
{
public:
	Symbol *name;

	Identifier(Symbol *_name);
	virtual string toString();
	virtual bool indexed(){ return false; }
};

class IndexedIdentifier:
	public Identifier
{
public:
	IntNum *index;

	IndexedIdentifier(Symbol *_name, IntNum *_index);
	virtual string toString();
	virtual bool indexed(){ return true; }
};

class Declaration:
	public Base
{
public:
	Type *type;
	list<Identifier *> identifier;

	Declaration(Type *_type, List *_identifier);
	virtual string toString();
	virtual void traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
};

class Parameter:
	public Base
{
public:
	Type *type;
	Identifier *identifier;

	Parameter(Type *_type, Identifier *_identifier);
	virtual string toString();
	virtual void traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
};

class Expr:
	public Base, public Pos
{
public:
	Expr(size_t _pos);
	virtual AbstractType *getType(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error) = 0;
};

class UnOpExpr:
	public Expr
{
public:
	enum UnOp {
		MINUS
	};

	UnOp unOp;
	Expr *expr;

	UnOpExpr(UnOp _unOp, Expr *_expr);
	virtual string toString();
	virtual AbstractType *getType(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
};

class BinOpExpr:
	public Expr
{
public:
	enum BinOp {
		PLUS, MINUS, MUL, DIV,
		EQ, NE, LT, LE, GT, GE
	};

	Expr *left;
	BinOp binOp;
	Expr *right;

	BinOpExpr(Expr *_left, BinOp _binOp, Expr *_right);
	virtual string toString();
	virtual AbstractType *getType(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
};

class IntNumExpr:
	public Expr
{
public:
	IntNum *intNum;

	IntNumExpr(IntNum *_intNum);
	virtual string toString();
	virtual AbstractType *getType(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
};

class FloatNumExpr:
	public Expr
{
public:
	FloatNum *floatNum;

	FloatNumExpr(FloatNum *_floatNum);
	virtual string toString();
	virtual AbstractType *getType(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
};

class TypeCastExpr:
	public Expr
{
public:
	Type *type;
	Expr *expr;

	TypeCastExpr(Type *_type, Expr *_expr);
	virtual string toString();
	virtual AbstractType *getType(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
};

class SymbolExpr:
	public Expr
{
public:
	Symbol *name;

	SymbolExpr(Symbol *_name);
	virtual string toString();
	virtual AbstractType *getType(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
	virtual bool indexed(){ return false; }
};

class IndexedSymbolExpr:
	public SymbolExpr
{
public:
	Expr *index;

	IndexedSymbolExpr(Symbol *_name, Expr *_index);
	virtual string toString();
	virtual AbstractType *getType(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
	virtual bool indexed(){ return true; }
};

class Stmt:
	public Base, public Pos
{
public:
	Stmt(size_t _pos);
	virtual string toString();
};

class Call:
	public Stmt, public Expr
{
public:
	Symbol *name;
	list<Expr *> expr;

	Call(Symbol *_name, List *_expr = NULL);
	virtual string toString();
	virtual AbstractType *getType(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
};

class CallStmt:
	public Stmt
{
public:
	Call *call;

	CallStmt(Call *_call);
	virtual string toString();
};

class Assign:
	public Stmt
{
public:
	Symbol *name;
	Expr *expr;

	Assign(Symbol *_name, Expr *_expr);
	virtual string toString();
	virtual void traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
};

class IndexedAssign:
	public Assign
{
public:
	Expr *index;

	IndexedAssign(Symbol *_name, Expr *_index, Expr *_expr);
	virtual string toString();
	virtual void traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
};

class AssignStmt:
	public Stmt
{
public:
	Assign *assign;

	AssignStmt(Assign *_assign);
	virtual string toString();
	virtual void traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
};

class RetStmt:
	public Stmt
{
public:
	Expr *expr;

	RetStmt(size_t _pos, Expr *_expr = NULL);
	virtual string toString();
	virtual void traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
};

class WhileStmt:
	public Stmt
{
public:
	Expr *expr;
	Stmt *stmt;

	WhileStmt(Expr *_expr, Stmt *_stmt);
	virtual string toString();
	virtual void traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
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
public:
	Assign *first;
	Expr *second;
	Assign *third;
	Stmt *stmt;

	ForStmt(Assign *_first, Expr *_second, Assign *_third, Stmt *_stmt);
	virtual string toString();
	virtual void traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
};

class IfStmt:
	public Stmt
{
public:
	Expr *expr;
	Stmt *than, *_else;

	IfStmt(Expr *_expr, Stmt *_than, Stmt *__else = NULL);
	virtual string toString();
	virtual void traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
};

class Case:
	public Base
{
public:
	IntNum *index;
	list<Stmt *> stmt;
	bool _break;

	Case(IntNum *_index, List *_stmt, bool __break);
	virtual string toString();
	virtual void traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
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
public:
	Expr *expr;
	list<Case *> _case;

	SwitchStmt(Identifier *_identifier, List *__case);
	virtual string toString();
	virtual void traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
};

class CompoundStmt:
	public Stmt
{
public:
	list<Declaration *> declaration;
	list<Stmt *> stmt;

	CompoundStmt(size_t _pos, List *_declaration, List *_stmt);
	virtual string toString();
	virtual void traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
};


class Function:
	public Base
{
public:
	Type *type;
	Symbol *name;
	list<Parameter *> parameter;
	CompoundStmt *compoundStmt;

	Function(Type *_type, Symbol *_name, List *_parameter, CompoundStmt *_compoundStmt);
	virtual string toString();
	virtual void traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
};

class Program:
	public Base
{
public:
	list<Declaration *> declaration;
	list<Function *> func;

	static Program *program;

	Program(List *_declaration, List *_func);
	virtual string toString();
	virtual void traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error);
};

class Scope
{
public:
	Scope *parent;
	list<pair<AbstractType *, Symbol *> > symbol;
	AbstractFunctionType *func;

	Scope(Scope *_parent, AbstractFunctionType *_func);
	AbstractType *getType(Symbol *_symbol);
};

class AbstractType
{
public:
	Type *type;
	bool indexed;

	AbstractType(Type *_type, bool _indexed = false);
	virtual bool func(){ return false; }
};

class AbstractFunctionType:
	public AbstractType
{
public:
	list<AbstractType *> parameter;

	AbstractFunctionType(Type *_type, bool _indexed = false);
	virtual bool func(){ return true; }
};


}

#endif
