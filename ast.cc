#include "ast.h"

namespace ast
{

Base::Base()
{
}

string Base::toString()
{
	return "";
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

string Symbol::toString()
{
	return symbol;
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

string Type::toString()
{
	switch(type)
	{
		case INT:
			return "int";
		case FLOAT:
			return "float";
	}
}

Identifier::Identifier(Symbol *_name)
	: Base(), name(_name)
{
}

string Identifier::toString()
{
	return name->toString();
}

IndexedIdentifier::IndexedIdentifier(Symbol *_name, IntNum *_index)
	: Identifier(_name), index(_index)
{
}

string IndexedIdentifier::toString()
{
	return name->toString() + "[" + index->toString() + "]";
}

Declaration::Declaration(Type *_type, List *_identifier)
	: Base(), type(_type)
{
	List *it;
	for(it = _identifier; it; it = it->next())
	{
		identifier.push_front((Identifier *)it->get());
	}
	delete _identifier;
}

string Declaration::toString()
{
	string res = type->toString() + " ";
	for(auto it = identifier.begin(); it != identifier.end(); it++)
	{
		if(it != identifier.begin())
		{
			res += ",";
		}
		res += (*it)->toString();
	}
	res += ";";
	return res;
}

Parameter::Parameter(Type *_type, Identifier *_identifier)
	: Base(), type(_type), identifier(_identifier)
{
}

string Parameter::toString()
{
	return type->toString() + " " + identifier->toString();
}

Expr::Expr()
	: Base()
{
}

UnOpExpr::UnOpExpr(UnOp _unOp, Expr *_expr)
	: Expr(), unOp(_unOp), expr(_expr)
{
}

string UnOpExpr::toString()
{
	switch(unOp)
	{
		case MINUS:
			return "-(" + expr->toString() + ")";
	}
}

BinOpExpr::BinOpExpr(Expr *_left, BinOp _binOp, Expr *_right)
	: Expr(), left(_left), binOp(_binOp), right(_right)
{
}

string BinOpExpr::toString()
{
	string op;
	switch(binOp)
	{
		case PLUS:
			op = "+";
			break;
		case MINUS:
			op = "-";
			break;
		case MUL:
			op = "*";
			break;
		case DIV:
			op = "/";
			break;
		case EQ:
			op = "==";
			break;
		case NE:
			op = "!=";
			break;
		case LT:
			op = "<";
			break;
		case LE:
			op = "<=";
			break;
		case GT:
			op = ">";
			break;
		case GE:
			op = ">=";
			break;
	}
	return "(" + left->toString() + ")" + op + "(" + right->toString() + ")";
}

IntNumExpr::IntNumExpr(IntNum *_intNum)
	: Expr(), intNum(_intNum)
{
}

string IntNumExpr::toString()
{
	return intNum->toString();
}

FloatNumExpr::FloatNumExpr(FloatNum *_floatNum)
	: Expr(), floatNum(_floatNum)
{
}

string FloatNumExpr::toString()
{
	return floatNum->toString();
}

SymbolExpr::SymbolExpr(Symbol *_name)
	: Expr(), name(_name)
{
}

string SymbolExpr::toString()
{
	return name->toString();
}

IndexedSymbolExpr::IndexedSymbolExpr(Symbol *_name, Expr *_index)
	: SymbolExpr(_name), index(_index)
{
}

string IndexedSymbolExpr::toString()
{
	return name->toString() + "[" + index->toString() + "]";
}

Stmt::Stmt()
	: Base()
{
}

string Stmt::toString()
{
	return ";";
}

Call::Call(Symbol *_name, List *_expr)
	: Stmt(), Expr(), name(_name)
{
	List *it;
	for(it = _expr; it; it = it->next())
	{
		expr.push_front((Expr *)it->get());
	}
	delete _expr;
}

string Call::toString()
{
	string res = name->toString() + "(";
	for(auto it = expr.begin(); it != expr.end(); it++)
	{
		if(it != expr.begin())
		{
			res += ",";
		}
		res += (*it)->toString();
	}
	res += ")";
	return res;
}

CallStmt::CallStmt(Call *_call)
	: Stmt(), call(_call)
{
}

string CallStmt::toString()
{
	return call->toString() + ";";
}

Assign::Assign(Symbol *_name, Expr *_expr)
	: Stmt(), name(_name), expr(_expr)
{
}

string Assign::toString()
{
	return name->toString() + "=" + expr->toString();
}

IndexedAssign::IndexedAssign(Symbol *_name, Expr *_index, Expr *_expr)
	: Assign(_name, _expr), index(_index)
{
}

string IndexedAssign::toString()
{
	return name->toString() + "[" + index->toString() + "]=" + expr->toString();
}

AssignStmt::AssignStmt(Assign *_assign)
	: Stmt(), assign(_assign)
{
}

string AssignStmt::toString()
{
	return assign->toString() + ";";
}

RetStmt::RetStmt(Expr *_expr)
	: Stmt(), expr(_expr)
{
}

string RetStmt::toString()
{
	if(expr)
	{
		return "return " + expr->toString() + ";";
	}
	else
	{
		return "return;";
	}
}

WhileStmt::WhileStmt(Expr *_expr, Stmt *_stmt)
	: Stmt(), expr(_expr), stmt(_stmt)
{
}

string WhileStmt::toString()
{
	return "while(" + expr->toString() + ")\n" + stmt->toString();
}

DoWhileStmt::DoWhileStmt(Expr *_expr, Stmt *_stmt)
	: WhileStmt(_expr, _stmt)
{
}

string DoWhileStmt::toString()
{
	return "do\n" + stmt->toString() + "\nwhile(" + expr->toString() + ");";
}

ForStmt::ForStmt(Assign *_first, Expr *_second, Assign *_third, Stmt *_stmt)
	: Stmt(), first(_first), second(_second), third(_third), stmt(_stmt)
{
}

string ForStmt::toString()
{
	return "for(" + first->toString() + ";" + second->toString() + ";" + third->toString() + ")\n" + stmt->toString();
}

IfStmt::IfStmt(Expr *_expr, Stmt *_than, Stmt *__else)
	: Stmt(), expr(_expr), than(_than), _else(__else)
{
}

string IfStmt::toString()
{
	string res = "if(" + expr->toString() + ")\n" + than->toString();
	if(_else)
	{
		res += "\nelse\n" + _else->toString();
	}
	return res;
}

Case::Case(IntNum *_index, List *_stmt, bool __break)
	: Base(), index(_index), _break(__break)
{
	List *it;
	for(it = _stmt; it; it = it->next())
	{
		stmt.push_front((Stmt *)it->get());
	}
	delete _stmt;
}

string Case::toString()
{
	string res = "case " + index->toString() + ":";
	for(auto it = stmt.begin(); it != stmt.end(); it++)
	{
		res += "\n" + (*it)->toString();
	}
	if(_break)
	{
		res += "\nbreak;";
	}
	return res;
}

DefaultCase::DefaultCase(List *_stmt, bool __break)
	: Case(NULL, _stmt, __break)
{
}

string DefaultCase::toString()
{
	string res = "default:";
	for(auto it = stmt.begin(); it != stmt.end(); it++)
	{
		res += "\n" + (*it)->toString();
	}
	if(_break)
	{
		res += "\nbreak";
	}
	return res;
}

SwitchStmt::SwitchStmt(Identifier *_identifier, List *__case)
	: Stmt(), identifier(_identifier)
{
	List *it;
	for(it = __case; it; it = it->next())
	{
		_case.push_front((Case *)it->get());
	}
	delete __case;
}

string SwitchStmt::toString()
{
	string res = "switch(" + identifier->toString() + ")\n{";
	for(auto it = _case.begin(); it != _case.end(); it++)
	{
		res += "\n" + (*it)->toString();
	}
	res += "\n}";
	return res;
}

CompoundStmt::CompoundStmt(List *_declaration, List *_stmt)
	: Stmt()
{
	List *it;
	for(it = _declaration; it; it = it->next())
	{
		declaration.push_front((Declaration *)it->get());
	}
	for(it = _stmt; it; it= it->next())
	{
		stmt.push_front((Stmt *)it->get());
	}
	delete _declaration, _stmt;
}

string CompoundStmt::toString()
{
	string res = "{";
	for(auto it = declaration.begin(); it != declaration.end(); it++)
	{
		res += "\n" + (*it)->toString();
	}
	for(auto it = stmt.begin(); it != stmt.end(); it++)
	{
		res += "\n" + (*it)->toString();
	}
	res += "\n}";
	return res;
}

Function::Function(Type *_type, Symbol *_name, List *_parameter, CompoundStmt *_compoundStmt)
	: Base(), type(_type), name(_name), compoundStmt(_compoundStmt)
{
	List *it;
	for(it = _parameter; it; it = it->next())
	{
		parameter.push_front((Parameter *)it->get());
	}
	delete _parameter;
}

string Function::toString()
{
	string res = type->toString() + " " + name->toString() + "(";
	for(auto it = parameter.begin(); it != parameter.end(); it++)
	{
		if(it != parameter.begin())
		{
			res += ",";
		}
		res += (*it)->toString();
	}
	res += ")\n" + compoundStmt->toString();
	return res;
}

Program::Program(List *_declaration, List *_function)
	: Base()
{
	List *it;
	for(it = _declaration; it; it = it->next())
	{
		declaration.push_front((Declaration *)it->get());
	}
	for(it = _function; it; it= it->next())
	{
		function.push_front((Function *)it->get());
	}
	delete _declaration, _function;
}

string Program::toString()
{
	string res = "";
	for(auto it = declaration.begin(); it != declaration.end(); it++)
	{
		res += (*it)->toString() + "\n";
	}
	for(auto it = function.begin(); it != function.end(); it++)
	{
		res += (*it)->toString() + "\n";
	}
	return res;
}

}
