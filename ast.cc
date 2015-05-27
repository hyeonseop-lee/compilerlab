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

void Base::traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
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

Pos::Pos(size_t _pos)
	: pos(_pos)
{
}

List *List::next()
{
	return tail;
}

Symbol::Symbol(size_t _pos, string _symbol)
	: Base(), Pos(_pos), symbol(_symbol)
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

void Declaration::traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	for(auto it = identifier.begin(); it != identifier.end(); it++)
	{
		scope->symbol.push_back(make_pair(new AbstractType(type, (*it)->indexed()), (*it)->name));
	}
}

Parameter::Parameter(Type *_type, Identifier *_identifier)
	: Base(), type(_type), identifier(_identifier)
{
}

string Parameter::toString()
{
	return type->toString() + " " + identifier->toString();
}

void Parameter::traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	scope->symbol.push_back(make_pair(new AbstractType(type, identifier->indexed()), identifier->name));
}

Expr::Expr(size_t _pos)
	: Base(), Pos(_pos)
{
}

UnOpExpr::UnOpExpr(UnOp _unOp, Expr *_expr)
	: Expr(_expr->pos), unOp(_unOp), expr(_expr)
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

AbstractType *UnOpExpr::getType(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	AbstractType *type = expr->getType(scope, warning, error);
	if(type->indexed) error(pos, "Type error", expr->toString() + " is an array");
	if(type->func()) error(pos, "Type error", expr->toString() + " is a function");
	return type;
}

BinOpExpr::BinOpExpr(Expr *_left, BinOp _binOp, Expr *_right)
	: Expr(_left->pos), left(_left), binOp(_binOp), right(_right)
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

AbstractType *BinOpExpr::getType(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	AbstractType *_left = left->getType(scope, warning, error), *_right = right->getType(scope, warning, error);
	if(_left->indexed) error(pos, "Type error", left->toString() + " is an array");
	if(_left->func()) error(pos, "Type error", left->toString() + " is a function");
	if(_right->indexed) error(pos, "Type error", right->toString() + " is an array");
	if(_right->func()) error(pos, "Type error", right->toString() + " is a function");
	if(_left->type->type == Type::FLOAT || _right->type->type == Type::FLOAT)
	{
		if(_left->type->type == Type::INT)
		{
			warning(pos, "Implicit type casting", toString());
			left = new TypeCastExpr(new Type(Type::FLOAT), left);
		}
		if(_right->type->type == Type::INT)
		{
			warning(pos, "Implicit type casting", toString());
			right = new TypeCastExpr(new Type(Type::FLOAT), right);
		}
		switch(binOp)
		{
			case PLUS:
			case MINUS:
			case MUL:
			case DIV:
				return new AbstractType(new Type(Type::FLOAT));
			case EQ:
			case NE:
			case LT:
			case LE:
			case GT:
			case GE:
			default:
				return new AbstractType(new Type(Type::INT));
		}
	}
	return new AbstractType(new Type(Type::INT));
}

IntNumExpr::IntNumExpr(IntNum *_intNum)
	: Expr(_intNum->pos), intNum(_intNum)
{
}

string IntNumExpr::toString()
{
	return intNum->toString();
}

AbstractType *IntNumExpr::getType(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	return new AbstractType(new Type(Type::INT));
}

FloatNumExpr::FloatNumExpr(FloatNum *_floatNum)
	: Expr(_floatNum->pos), floatNum(_floatNum)
{
}

string FloatNumExpr::toString()
{
	return floatNum->toString();
}

AbstractType *FloatNumExpr::getType(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	return new AbstractType(new Type(Type::FLOAT));
}

TypeCastExpr::TypeCastExpr(Type *_type, Expr *_expr)
	: Expr(_expr->pos), type(_type), expr(_expr)
{
}

string TypeCastExpr::toString()
{
	return type->toString() + "(" + expr->toString() + ")";
}

AbstractType *TypeCastExpr::getType(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	AbstractType *_type = expr->getType(scope, warning, error);
	if(_type->indexed) error(pos, "Type error", expr->toString() + " is an array");
	if(_type->func()) error(pos, "Type error", expr->toString() + " is a function");
	return new AbstractType(type);
}

SymbolExpr::SymbolExpr(Symbol *_name)
	: Expr(_name->pos), name(_name)
{
}

string SymbolExpr::toString()
{
	return name->toString();
}

AbstractType *SymbolExpr::getType(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	AbstractType *type = scope->getType(name);
	if(type == NULL) error(pos, "Not defined", name->toString());
	return type;
}

IndexedSymbolExpr::IndexedSymbolExpr(Symbol *_name, Expr *_index)
	: SymbolExpr(_name), index(_index)
{
}

string IndexedSymbolExpr::toString()
{
	return name->toString() + "[" + index->toString() + "]";
}

AbstractType *IndexedSymbolExpr::getType(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	AbstractType *type = scope->getType(name);
	if(type == NULL) error(pos, "Not defined", name->toString());
	if(!type->indexed) error(pos, "Type error", name->toString() + " is not an array");
	return new AbstractType(type->type);
}

Stmt::Stmt(size_t _pos)
	: Pos(_pos)
{
}

string Stmt::toString()
{
	return string(";");
}

Call::Call(Symbol *_name, List *_expr)
	: Stmt(_name->pos), Expr(_name->pos), name(_name)
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

AbstractType *Call::getType(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	bool casting = false;
	AbstractType *type = scope->getType(name);
	AbstractFunctionType *func;
	list<AbstractType *> ::iterator it;
	list<Expr *> ::iterator jt;
	if(type == NULL) error(name->pos, "Not defined", name->toString());
	if(type->indexed) error(name->pos, "Type error", name->toString() + " is an array");
	if(!type->func()) error(name->pos, "Type error", name->toString() + " is not a function");
	func = (AbstractFunctionType *)type;
	for(it = func->parameter.begin(), jt = expr.begin(); it != func->parameter.end() && jt != expr.end(); it++, jt++)
	{
		type = (*jt)->getType(scope, warning, error);
		if((*it)->func() || type->func() || (*it)->indexed != type->indexed)
		{
			break;
		}
		if((*it)->type->type != type->type->type)
		{
			casting = true;
			*jt = new TypeCastExpr((*it)->type, *jt);
		}
	}
	if(it != func->parameter.end() || jt != expr.end())
	{
		error(name->pos, "Type error", toString());
	}
	if(casting)
	{
		warning(name->pos, "Implicit type casting", toString());
	}
	return new AbstractType(func->type);
}

CallStmt::CallStmt(Call *_call)
	: Stmt(_call->name->pos), call(_call)
{
}

string CallStmt::toString()
{
	return call->toString() + ";";
}

Assign::Assign(Symbol *_name, Expr *_expr)
	: Stmt(_name->pos), name(_name), expr(_expr)
{
}

string Assign::toString()
{
	return name->toString() + "=" + expr->toString();
}

void Assign::traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	AbstractType *left = scope->getType(name), *right = expr->getType(scope, warning, error);
	if(left == NULL) error(pos, "Not defined", name->toString());
	if(left->indexed) error(pos, "Type error", name->toString() + " is an array");
	if(left->func()) error(pos, "Type error", name->toString() + " is a function");
	if(right->indexed) error(pos, "Type error", expr->toString() + " is an array");
	if(right->func()) error(pos, "Type error", expr->toString() + " is a function");
	if(left->type->type != right->type->type)
	{
		warning(pos, "Implicit type casting", toString());
		expr = new TypeCastExpr(left->type, expr);
	}
}

IndexedAssign::IndexedAssign(Symbol *_name, Expr *_index, Expr *_expr)
	: Assign(_name, _expr), index(_index)
{
}

string IndexedAssign::toString()
{
	return name->toString() + "[" + index->toString() + "]=" + expr->toString();
}

void IndexedAssign::traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	AbstractType *left = scope->getType(name), *right = expr->getType(scope, warning, error), *_index = index->getType(scope, warning, error);
	if(!left->indexed) error(pos, "Type error", name->toString() + " is not an array");
	if(_index->indexed) error(pos, "Type error", expr->toString() + " is an array");
	if(_index->func()) error(pos, "Type error", expr->toString() + " is a function");
	if(_index->type->type != Type::INT)
	{
		warning(pos, "Implicit type casting", name->toString() + "[" + index->toString() + "]");
		index = new TypeCastExpr(new Type(Type::INT), index);
	}
	if(left->func()) error(pos, "Type error", name->toString() + "[" + index->toString() + "] is a function");
	if(right->indexed) error(pos, "Type error", expr->toString() + " is an array");
	if(right->func()) error(pos, "Type error", expr->toString() + " is a function");
	if(left->type->type != right->type->type)
	{
		warning(pos, "Implicit type casting", toString());
		expr = new TypeCastExpr(left->type, expr);
	}
}

AssignStmt::AssignStmt(Assign *_assign)
	: Stmt(_assign->name->pos), assign(_assign)
{
}

string AssignStmt::toString()
{
	return assign->toString() + ";";
}

void AssignStmt::traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	assign->traverse(scope, warning, error);
}

RetStmt::RetStmt(size_t _pos, Expr *_expr)
	: Stmt(_pos), expr(_expr)
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

void RetStmt::traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	AbstractType *type;
	if(expr == NULL) error(pos, "Type error", "Should return a value");
	type = expr->getType(scope, warning, error);
	if(type->indexed) error(pos, "Type error", expr->toString() + " is an array");
	if(type->func()) error(pos, "Type error", expr->toString() + " is a function");
	if(type->type->type != scope->func->type->type)
	{
		warning(pos, "Implicit type casting", toString());
		expr = new TypeCastExpr(scope->func->type, expr);
	}
}

WhileStmt::WhileStmt(Expr *_expr, Stmt *_stmt)
	: Stmt(_expr->pos), expr(_expr), stmt(_stmt)
{
}

string WhileStmt::toString()
{
	return "while(" + expr->toString() + ")\n" + stmt->toString();
}

void WhileStmt::traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	AbstractType *type = expr->getType(scope, warning, error);
	if(type->indexed) error(pos, "Type error", expr->toString() + " is an array");
	if(type->func()) error(pos, "Type error", expr->toString() + " is a function");
	if(type->type->type == Type::FLOAT)
	{
		warning(pos, "Implicit type casting", string("while(") + expr->toString() + ")");
		expr = new TypeCastExpr(new Type(Type::INT), expr);
	}
	stmt->traverse(scope, warning, error);
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
	: Stmt(_first->name->pos), first(_first), second(_second), third(_third), stmt(_stmt)
{
}

string ForStmt::toString()
{
	return "for(" + first->toString() + ";" + second->toString() + ";" + third->toString() + ")\n" + stmt->toString();
}

void ForStmt::traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	AbstractType *type;
	first->traverse(scope, warning, error);
	type = second->getType(scope, warning, error);
	if(type->indexed) error(pos, "Type error", second->toString() + " is an array");
	if(type->func()) error(pos, "Type error", second->toString() + " is a function");
	if(type->type->type == Type::FLOAT)
	{
		warning(pos, "Implicit type casting", string("for(") + first->toString() + ";" + second->toString() + ";" + third->toString() + ")");
		second = new TypeCastExpr(new Type(Type::INT), second);
	}
	third->traverse(scope, warning, error);
	stmt->traverse(scope, warning, error);
}

IfStmt::IfStmt(Expr *_expr, Stmt *_than, Stmt *__else)
	: Stmt(_expr->pos), expr(_expr), than(_than), _else(__else)
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

void IfStmt::traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	AbstractType *type;
	type = expr->getType(scope, warning, error);
	if(type->indexed) error(pos, "Type error", expr->toString() + " is an array");
	if(type->func()) error(pos, "Type error", expr->toString() + " is a function");
	if(type->type->type == Type::FLOAT)
	{
		warning(pos, "Implicit type casting", "if(" + expr->toString() + ")");
		expr = new TypeCastExpr(new Type(Type::INT), expr);
	}
	than->traverse(scope, warning, error);
	if(_else)
	{
		_else->traverse(scope, warning, error);
	}
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

void Case::traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	for(auto it = stmt.begin(); it != stmt.end(); it++)
	{
		(*it)->traverse(scope, warning, error);
	}
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
	: Stmt(_identifier->name->pos)
{
	List *it;
	if(_identifier->indexed())
	{
		expr = new IndexedSymbolExpr(_identifier->name, new IntNumExpr(((IndexedIdentifier *)_identifier)->index));
	}
	else
	{
		expr = new SymbolExpr(_identifier->name);
	}
	delete _identifier;
	for(it = __case; it; it = it->next())
	{
		_case.push_front((Case *)it->get());
	}
	delete __case;
}

string SwitchStmt::toString()
{
	string res = "switch(" + expr->toString() + ")\n{";
	for(auto it = _case.begin(); it != _case.end(); it++)
	{
		res += "\n" + (*it)->toString();
	}
	res += "\n}";
	return res;
}

void SwitchStmt::traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	AbstractType *type = expr->getType(scope, warning, error);
	if(type->indexed) error(pos, "Type error", expr->toString() + " is an array");
	if(type->func()) error(pos, "Type error", expr->toString() + " is a function");
	if(type->type->type == Type::FLOAT)
	{
		warning(pos, "Implicit type casting", "switch(" + expr->toString() + ")");
		expr = new TypeCastExpr(new Type(Type::INT), expr);
	}
	for(auto it = _case.begin(); it != _case.end(); it++)
	{
		(*it)->traverse(scope, warning, error);
	}
}

CompoundStmt::CompoundStmt(size_t _pos, List *_declaration, List *_stmt)
	: Stmt(_pos)
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

void CompoundStmt::traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	scope = new Scope(scope, scope->func);
	for(auto it = declaration.begin(); it != declaration.end(); it++)
	{
		(*it)->traverse(scope, warning, error);
	}
	for(auto it = stmt.begin(); it != stmt.end(); it++)
	{
		(*it)->traverse(scope, warning, error);
	}
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

void Function::traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	AbstractFunctionType *func = new AbstractFunctionType(type);

	for(auto it = parameter.begin(); it != parameter.end(); it++)
	{
		func->parameter.push_back(new AbstractType((*it)->type, (*it)->identifier->indexed()));
	}
	scope->symbol.push_back(make_pair(func, name));
	scope = new Scope(scope, func);

	for(auto it = parameter.begin(); it != parameter.end(); it++)
	{
		(*it)->traverse(scope, warning, error);
	}
	for(auto it = compoundStmt->declaration.begin(); it != compoundStmt->declaration.end(); it++)
	{
		(*it)->traverse(scope, warning, error);
	}
	for(auto it = compoundStmt->stmt.begin(); it != compoundStmt->stmt.end(); it++)
	{
		(*it)->traverse(scope, warning, error);
	}
}

Program::Program(List *_declaration, List *_func)
	: Base()
{
	List *it;
	for(it = _declaration; it; it = it->next())
	{
		declaration.push_front((Declaration *)it->get());
	}
	for(it = _func; it; it= it->next())
	{
		func.push_front((Function *)it->get());
	}
	delete _declaration, _func;
}

string Program::toString()
{
	string res = "";
	for(auto it = declaration.begin(); it != declaration.end(); it++)
	{
		res += (*it)->toString() + "\n";
	}
	for(auto it = func.begin(); it != func.end(); it++)
	{
		res += (*it)->toString() + "\n";
	}
	return res;
}

void Program::traverse(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	scope = new Scope(scope, scope->func);
	for(auto it = declaration.begin(); it != declaration.end(); it++)
	{
		(*it)->traverse(scope, warning, error);
	}
	for(auto it = func.begin(); it != func.end(); it++)
	{
		(*it)->traverse(scope, warning, error);
	}
}

Scope::Scope(Scope *_parent, AbstractFunctionType *_func)
	: parent(_parent), func(_func)
{
}

AbstractType *Scope::getType(Symbol *_symbol)
{
	for(auto it = symbol.begin(); it != symbol.end(); it++)
	{
		if(it->second->symbol == _symbol->symbol)
		{
			return it->first;
		}
	}
	if(parent)
	{
		return parent->getType(_symbol);
	}
	return NULL;
}

AbstractType::AbstractType(Type *_type, bool _indexed)
	: type(_type), indexed(_indexed)
{
}

AbstractFunctionType::AbstractFunctionType(Type *_type, bool _indexed)
	: AbstractType(_type, _indexed)
{
}


}
