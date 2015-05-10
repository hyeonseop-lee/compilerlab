#include "ast.h"

namespace ast
{

Base::Base()
{
}

List::List(Base *_head, List *_tail)
	: head(_head), tail(_tail)
{
}

Base *List::get()
{
	return head;
}

List *List::next()
{
	return tail;
}

Symbol::Symbol(size_t _pos, string _symbol)
	: Base(), pos(_pos), symbol(_symbol)
{
}

IntNum::IntNum(size_t _pos, string _symbol)
	: Symbol(_pos, _symbol)
{
	num = stoi(symbol);
}

FloatNum::FloatNum(size_t _pos, string _symbol)
	: Symbol(_pos, _symbol)
{
	num = stof(symbol);
}

Type::Type(_Type _type)
	: Base(), type(_type)
{
}

Identifier::Identifier(Symbol *_name)
	: Base(), name(_name)
{
}

IndexedIdentifier::IndexedIdentifier(Symbol *_name, IntNum *_index)
	: Identifier(_name), index(_index)
{
}

Declaration::Declaration(Type *_type, List *_identifier)
	: Base(), type(_type)
{
	List *it;
	for(it = _identifier; it; it = it->next())
	{
		identifier.push_back((Identifier *)it->get());
	}
	delete _identifier;
}

Parameter::Parameter(Type *_type, Identifier *_identifier)
	: Base(), type(_type), identifier(_identifier)
{
}

Expr::Expr()
	: Base()
{
}

UnOpExpr::UnOpExpr(UnOp _unOp, Expr *_expr)
	: Expr(), unOp(_unOp), expr(_expr)
{
}

BinOpExpr::BinOpExpr(Expr *_left, BinOp _binOp, Expr *_right)
	: Expr(), left(_left), binOp(_binOp), right(_right)
{
}

IntNumExpr::IntNumExpr(IntNum *_intNum)
	: Expr(), intNum(_intNum)
{
}

FloatNumExpr::FloatNumExpr(FloatNum *_floatNum)
	: Expr(), floatNum(_floatNum)
{
}

SymbolExpr::SymbolExpr(Symbol *_name)
	: Expr(), name(_name)
{
}

IndexedSymbolExpr::IndexedSymbolExpr(Symbol *_name, Expr *_index)
	: SymbolExpr(_name), index(_index)
{
}

Stmt::Stmt()
	: Base()
{
}

Call::Call(Symbol *_name, List *_expr)
	: Stmt(), Expr(), name(_name)
{
	List *it;
	for(it = _expr; it; it = it->next())
	{
		expr.push_back((Expr *)it->get());
	}
	delete _expr;
}

CallStmt::CallStmt(Call *_call)
	: Stmt(), call(_call)
{
}

Assign::Assign(Symbol *_name, Expr *_expr)
	: Stmt(), name(_name), expr(_expr)
{
}

IndexedAssign::IndexedAssign(Symbol *_name, Expr *_index, Expr *_expr)
	: Assign(_name, _expr), index(_index)
{
}

AssignStmt::AssignStmt(Assign *_assign)
	: Stmt(), assign(_assign)
{
}

RetStmt::RetStmt(Expr *_expr)
	: Stmt(), expr(_expr)
{
}

WhileStmt::WhileStmt(Expr *_expr, Stmt *_stmt)
	: Stmt(), expr(_expr), stmt(_stmt)
{
}

DoWhileStmt::DoWhileStmt(Expr *_expr, Stmt *_stmt)
	: WhileStmt(_expr, _stmt)
{
}

ForStmt::ForStmt(Assign *_first, Expr *_second, Assign *_third, Stmt *_stmt)
	: Stmt(), first(_first), second(_second), third(_third), stmt(_stmt)
{
}

IfStmt::IfStmt(Expr *_expr, Stmt *_than, Stmt *__else)
	: Stmt(), expr(_expr), than(_than), _else(__else)
{
}

Case::Case(IntNum *_index, List *_stmt, bool __break)
	: Base(), index(_index), _break(__break)
{
	List *it;
	for(it = _stmt; it; it = it->next())
	{
		stmt.push_back((Stmt *)it->get());
	}
	delete _stmt;
}

DefaultCase::DefaultCase(List *_stmt, bool __break)
	: Case(NULL, _stmt, __break)
{
}

SwitchStmt::SwitchStmt(Identifier *_identifier, List *__case)
	: Stmt(), identifier(_identifier)
{
	List *it;
	for(it = __case; it; it = it->next())
	{
		_case.push_back((Case *)it->get());
	}
	delete __case;
}

CompoundStmt::CompoundStmt(List *_declaration, List *_stmt)
	: Stmt()
{
	List *it;
	for(it = _declaration; it; it = it->next())
	{
		declaration.push_back((Declaration *)it->get());
	}
	for(it = _stmt; it; it= it->next())
	{
		stmt.push_back((Stmt *)it->get());
	}
	delete _declaration, _stmt;
}

Function::Function(Type *_type, Symbol *_name, List *_parameter, CompoundStmt *_compoundStmt)
	: Base(), type(_type), name(_name), compoundStmt(_compoundStmt)
{
	List *it;
	for(it = _parameter; it; it = it->next())
	{
		parameter.push_back((Parameter *)it->get());
	}
	delete _parameter;
}

Program::Program(List *_declaration, List *_function)
	: Base()
{
	List *it;
	for(it = _declaration; it; it = it->next())
	{
		declaration.push_back((Declaration *)it->get());
	}
	for(it = _function; it; it= it->next())
	{
		function.push_back((Function *)it->get());
	}
	delete _declaration, _function;
}

}
