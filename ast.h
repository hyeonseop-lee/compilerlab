#ifndef _AST_H_
#define _AST_H_

#include <string>
#include <list>
#include <map>

namespace ast
{

class Traversal;
class Entry;
class Level;

using namespace std;

class Base
{
public:
	Base();

	virtual string toString();
	virtual void traverse(Traversal *traversal, int level, Level *current);
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
	public Base
{
public:
	size_t pos;
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
	virtual void traverseWithType(Traversal *traversal, int level, Level *current, Type *type);
};

class IndexedIdentifier:
	public Identifier
{
public:
	IntNum *index;

	IndexedIdentifier(Symbol *_name, IntNum *_index);
	virtual string toString();
	virtual void traverseWithType(Traversal *traversal, int level, Level *current, Type *type);
};

class Declaration:
	public Base
{
public:
	Type *type;
	list<Identifier *> identifier;

	Declaration(Type *_type, List *_identifier);
	virtual string toString();
	virtual void traverse(Traversal *traversal, int level, Level *current);
};

class Parameter:
	public Base
{
public:
	Type *type;
	Identifier *identifier;

	Parameter(Type *_type, Identifier *_identifier);
	virtual string toString();
	virtual void traverse(Traversal *traversal, int level, Level *current);
};

class Expr:
	public Base
{
public:
	Expr();
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
};

class IntNumExpr:
	public Expr
{
public:
	IntNum *intNum;

	IntNumExpr(IntNum *_intNum);
	virtual string toString();
};

class FloatNumExpr:
	public Expr
{
public:
	FloatNum *floatNum;

	FloatNumExpr(FloatNum *_floatNum);
	virtual string toString();
};

class SymbolExpr:
	public Expr
{
public:
	Symbol *name;

	SymbolExpr(Symbol *_name);
	virtual string toString();
};

class IndexedSymbolExpr:
	public SymbolExpr
{
public:
	Expr *index;

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
public:
	Symbol *name;
	list<Expr *> expr;

	Call(Symbol *_name, List *_expr = NULL);
	virtual string toString();
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
};

class IndexedAssign:
	public Assign

{
public:
	Expr *index;

	IndexedAssign(Symbol *_name, Expr *_index, Expr *_expr);
	virtual string toString();
};

class AssignStmt:
	public Stmt
{
public:
	Assign *assign;

	AssignStmt(Assign *_assign);
	virtual string toString();
};

class RetStmt:
	public Stmt
{
public:
	Expr *expr;

	RetStmt(Expr *_expr = NULL);
	virtual string toString();
};

class WhileStmt:
	public Stmt
{
public:
	Expr *expr;
	Stmt *stmt;

	WhileStmt(Expr *_expr, Stmt *_stmt);
	virtual string toString();
	virtual void traverse(Traversal *traversal, int level, Level *current);
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
	virtual void traverse(Traversal *traversal, int level, Level *current);
};

class IfStmt:
	public Stmt
{
public:
	Expr *expr;
	Stmt *than, *_else;

	IfStmt(Expr *_expr, Stmt *_than, Stmt *__else = NULL);
	virtual string toString();
	virtual void traverse(Traversal *traversal, int level, Level *table);
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
	virtual void traverse(Traversal *traversal, int level, Level *current);
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
	Identifier *identifier;
	list<Case *> _case;

	SwitchStmt(Identifier *_identifier, List *__case);
	virtual string toString();
	virtual void traverse(Traversal *traversal, int level, Level *current);
};

class CompoundStmt:
	public Stmt
{
public:
	list<Declaration *> declaration;
	list<Stmt *> stmt;

	CompoundStmt(List *_declaration, List *_stmt);
	virtual string toString();
	virtual void traverse(Traversal *traversal, int level, Level *current);
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
	virtual void traverse(Traversal *traversal, int level, Level *current);
};

class Program:
	public Base
{
public:
	list<Declaration *> declaration;
	list<Function *> function;

	static Program *program;

	Program(List *_declaration, List *_function);
	virtual string toString();
	virtual void traverse(Traversal *traversal, int level, Level *current);
};

class Traversal
{
public:
	map<int, list<Level *> > level;

	Traversal();
};

class Entry
{
public:
	string type, name, location;

	Entry();
	Entry(string _type, string _name, string _location);
};

class Level
{
public:
	list<Entry> entry;

	Level();
};


}

#endif
