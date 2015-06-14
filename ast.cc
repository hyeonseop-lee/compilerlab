#include "ast.h"

#include <queue>

namespace ast
{

Base::Base()
{
}

string Base::toString()
{
	return "";
}

int Base::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
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

int Declaration::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	int size = 0;
	for(auto it = identifier.begin(); it != identifier.end(); it++)
	{
		scope->addVar((*it)->name, scope->size + size + 1);
		size++;
		if((*it)->indexed())
		{
			printf("MOVE MEM(FP@)(%d) MEM(FP@)(%d)\n", scope->size + size + 1, scope->size + size);
			size += ((IndexedIdentifier *)(*it))->index->num;
		}
		scope->symbol.push_back(make_pair(new AbstractType(type, (*it)->indexed()), (*it)->name));
	}
	if(size)
	{
		printf("ADD SP@ %d SP\n", size);
		scope->size += size;
	}
	return 0;
}

Parameter::Parameter(Type *_type, Identifier *_identifier)
	: Base(), type(_type), identifier(_identifier)
{
}

string Parameter::toString()
{
	return type->toString() + " " + identifier->toString();
}

int Parameter::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	scope->addVar(identifier->name, scope->size - 2);
	scope->size--;
	scope->symbol.push_back(make_pair(new AbstractType(type, identifier->indexed()), identifier->name));
	return 0;
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

int UnOpExpr::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	int reg = expr->toInst(scope, warning, error);
	if(getType(scope, warning, error)->type->type == Type::INT)
	{
		printf("SUB 0 VR(%d)@ VR(%d)\n", reg, reg);
	}
	else
	{
		printf("FSUB 0.0 VR(%d)@ VR(%d)\n", reg, reg);
	}
	return reg;
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

int BinOpExpr::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	int regl = left->toInst(scope, warning, error), regr = right->toInst(scope, warning, error), reg = scope->ralloc();
	int lt, lf, lj;
	scope->free(regl);
	scope->free(regr);
	if(left->getType(scope, warning, error)->type->type == Type::FLOAT)
	{
		printf("F");
	}
	switch(binOp)
	{
		case PLUS:
			printf("ADD VR(%d)@ VR(%d)@ VR(%d)\n", regl, regr, reg);
			return reg;
		case MINUS:
			printf("SUB VR(%d)@ VR(%d)@ VR(%d)\n", regl, regr, reg);
			return reg;
		case MUL:
			printf("MUL VR(%d)@ VR(%d)@ VR(%d)\n", regl, regr, reg);
			return reg;
		case DIV:
			printf("DIV VR(%d)@ VR(%d)@ VR(%d)\n", regl, regr, reg);
			return reg;
		default:
			break;
	}
	lt = scope->lalloc();
	lf = scope->lalloc();
	lj = scope->lalloc();
	switch(binOp)
	{
		case EQ:
			printf("SUB VR(%d)@ VR(%d)@ VR(%d)\n", regl, regr, reg);
			printf("JMPZ VR(%d)@ L%d\n", reg, lt);
			printf("JMP L%d\n", lf);
			break;
		case NE:
			printf("SUB VR(%d)@ VR(%d)@ VR(%d)\n", regl, regr, reg);
			printf("JMPZ VR(%d)@ L%d\n", reg, lf);
			printf("JMP L%d\n", lt);
			break;
		case LT:
			printf("SUB VR(%d)@ VR(%d)@ VR(%d)\n", regl, regr, reg);
			printf("JMPN VR(%d)@ L%d\n", reg, lt);
			printf("JMP L%d\n", lf);
			break;
		case LE:
			printf("SUB VR(%d)@ VR(%d)@ VR(%d)\n", regr, regl, reg);
			printf("JMPN VR(%d)@ L%d\n", reg, lf);
			printf("JMP L%d\n", lt);
			break;
		case GT:
			printf("SUB VR(%d)@ VR(%d)@ VR(%d)\n", regr, regl, reg);
			printf("JMPN VR(%d)@ L%d\n", reg, lt);
			printf("JMP L%d\n", lf);
			break;
		case GE:
			printf("SUB VR(%d)@ VR(%d)@ VR(%d)\n", regl, regr, reg);
			printf("JMPN VR(%d)@ L%d\n", reg, lf);
			printf("JMP L%d\n", lt);
			break;
		default:
			break;
	}
	printf("LAB L%d\n", lt);
	printf("MOVE 1 VR(%d)\n", reg);
	printf("JMP L%d\n", lj);
	printf("LAB L%d\n", lf);
	printf("MOVE 0 VR(%d)\n", reg);
	printf("LAB L%d\n", lj);
	return reg;
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

int IntNumExpr::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	int reg = scope->ralloc();
	printf("MOVE %d VR(%d)\n", intNum->num, reg);
	return reg;
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

int FloatNumExpr::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	int reg = scope->ralloc();
	printf("MOVE %.6f VR(%d)\n", floatNum->num, reg);
	return reg;
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

int TypeCastExpr::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	int reg = expr->toInst(scope, warning, error);
	if(expr->getType(scope, warning, error)->type->type == Type::INT)
	{
		if(type->type == Type::FLOAT)
		{
			printf("F2I VR(%d)@ VR(%d)\n", reg, reg);
		}
	}
	else
	{
		if(type->type == Type::INT)
		{
			printf("I2F VR(%d)@ VR(%d)\n", reg, reg);
		}
	}
	return reg;
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

int SymbolExpr::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	int reg = scope->ralloc();
	if(scope->isLocal(name))
	{
		printf("MOVE MEM(FP@(%d))@ VR(%d)\n", scope->getVar(name), reg);
	}
	else
	{
		printf("MOVE MEM(%d)@ VR(%d)\n", scope->getVar(name), reg);
	}
	return reg;
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

int IndexedSymbolExpr::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	int reg = scope->ralloc(), regi = index->toInst(scope, warning, error);
	if(scope->isLocal(name))
	{
		printf("MOVE MEM(FP@(%d))@(VR(%d)@)@ VR(%d)\n", scope->getVar(name), regi, reg);
	}
	else
	{
		printf("MOVE MEM(%d)@(VR(%d)@)@ VR(%d)\n", scope->getVar(name), regi, reg);
	}
	scope->free(regi);
	return reg;
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
	: Expr(_name->pos), name(_name)
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
	if(name->symbol == "printf" || name->symbol == "scanf")
	{
		error(name->pos, "Type error", toString());
	}
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

int Call::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	int reg, lr, stk = 0;
	SymbolExpr *symbol;
	for(auto it = scope->regs.begin(); it != scope->regs.end(); it++)
	{
		printf("MOVE VR(%d)@ MEM(SP@)(%d)\n", *it, ++stk);
	}
	if(name->symbol != "scanf")
	{
		for(auto it = expr.rbegin(); it != expr.rend(); it++)
		{
			reg = (*it)->toInst(scope, warning, error);
			printf("MOVE VR(%d)@ MEM(SP@)(%d)\n", reg, ++stk);
			scope->free(reg);
		}
	}
	else
	{
		printf("// calling scanf\n");
		symbol = (SymbolExpr *)*expr.begin();
		stk++;
		if(symbol->indexed())
		{
			reg = ((IndexedSymbolExpr *)symbol)->index->toInst(scope, warning, error);
			if(scope->isLocal(symbol->name))
			{
				printf("MOVE MEM(FP@(%d))@(VR(%d)@) MEM(SP@)(1)\n", scope->getVar(symbol->name), reg);
			}
			else
			{
				printf("MOVE MEM(%d)@(VR(%d)@) MEM(SP@)(1)\n", scope->getVar(symbol->name), reg);
			}
			scope->free(reg);
		}
		else
		{
			if(scope->isLocal(symbol->name))
			{
				printf("MOVE MEM(FP@(%d)) MEM(SP@)(1)\n", scope->getVar(symbol->name));
			}
			else
			{
				printf("MOVE MEM(%d) MEM(SP@)(1)\n", scope->getVar(symbol->name));
			}
		}
	}
	lr = scope->lalloc();
	printf("ADD SP@ %d SP\n", stk + 1);
	printf("MOVE L%d MEM(SP@)\n", lr);
	if(name->symbol == "scanf")
	{
		if(symbol->getType(scope, warning, error)->type->type == Type::INT)
		{
			printf("JMP Fscanfi\n");
		}
		else
		{
			printf("JMP Fscanff\n");
		}
	}
	else
	{
		printf("JMP F%s\n", name->symbol.c_str());
	}
	printf("LAB L%d\n", lr);
	reg = scope->ralloc();
	if(name->symbol != "printf" && name->symbol != "scanf")
	{
		printf("MOVE VR@ VR(%d)\n", reg);
	}
	printf("SUB SP@ %d SP\n", stk + 1);
	stk = 0;
	for(auto it = scope->regs.begin(); it != scope->regs.end(); it++)
	{
		if(*it != reg)
		{
			printf("MOVE MEM(SP@)(%d)@ VR(%d)\n", ++stk, *it);
		}
	}
	return reg;
}

CallStmt::CallStmt(Call *_call)
	: Stmt(_call->name->pos), call(_call)
{
}

string CallStmt::toString()
{
	return call->toString() + ";";
}

int CallStmt::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	if(call->expr.size() != 1 || (call->name->symbol != "printf" && call->name->symbol != "scanf"))
	{
		call->getType(scope, warning, error);
	}
	else
	{
		(*call->expr.begin())->getType(scope, warning, error);
	}
	scope->free(call->toInst(scope, warning, error));
}

Assign::Assign(Symbol *_name, Expr *_expr)
	: Stmt(_name->pos), name(_name), expr(_expr)
{
}

string Assign::toString()
{
	return name->toString() + "=" + expr->toString();
}

int Assign::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
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
	int reg = expr->toInst(scope, warning, error);
	if(scope->isLocal(name))
	{
		printf("MOVE VR(%d)@ MEM(FP@(%d))\n", reg, scope->getVar(name));
	}
	else
	{
		printf("MOVE VR(%d)@ MEM(%d)\n", reg, scope->getVar(name));
	}
	scope->free(reg);
}

IndexedAssign::IndexedAssign(Symbol *_name, Expr *_index, Expr *_expr)
	: Assign(_name, _expr), index(_index)
{
}

string IndexedAssign::toString()
{
	return name->toString() + "[" + index->toString() + "]=" + expr->toString();
}

int IndexedAssign::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
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

	int reg = expr->toInst(scope, warning, error), regi = index->toInst(scope, warning, error);
	if(scope->isLocal(name))
	{
		printf("MOVE VR(%d)@ MEM(FP@(%d))@(VR(%d)@)\n", reg, scope->getVar(name), regi);
	}
	else
	{
		printf("MOVE VR(%d)@ MEM(%d)@(VR(%d)@)\n", reg, scope->getVar(name), regi);
	}
	scope->free(reg);
	scope->free(regi);
}

AssignStmt::AssignStmt(Assign *_assign)
	: Stmt(_assign->name->pos), assign(_assign)
{
}

string AssignStmt::toString()
{
	return assign->toString() + ";";
}

int AssignStmt::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	assign->toInst(scope, warning, error);
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

int RetStmt::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	AbstractType *type;
	int reg;
	if(expr == NULL) error(pos, "Type error", "Should return a value");
	type = expr->getType(scope, warning, error);
	if(type->indexed) error(pos, "Type error", expr->toString() + " is an array");
	if(type->func()) error(pos, "Type error", expr->toString() + " is a function");
	if(type->type->type != scope->func->type->type)
	{
		warning(pos, "Implicit type casting", toString());
		expr = new TypeCastExpr(scope->func->type, expr);
	}
	reg = expr->toInst(scope, warning, error);
	printf("MOVE VR(%d)@ VR\n", reg);
	scope->free(reg);
	printf("MOVE FP@ SP\n");
	printf("MOVE MEM(SP@)@ FP\n");
	printf("SUB SP@ 1 SP\n");
	printf("JMP MEM(SP@)@\n");
	return 0;
}

WhileStmt::WhileStmt(Expr *_expr, Stmt *_stmt)
	: Stmt(_expr->pos), expr(_expr), stmt(_stmt)
{
}

string WhileStmt::toString()
{
	return "while(" + expr->toString() + ")\n" + stmt->toString();
}

int WhileStmt::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	AbstractType *type = expr->getType(scope, warning, error);
	int reg, lt, lf;
	if(type->indexed) error(pos, "Type error", expr->toString() + " is an array");
	if(type->func()) error(pos, "Type error", expr->toString() + " is a function");
	if(type->type->type == Type::FLOAT)
	{
		warning(pos, "Implicit type casting", string("while(") + expr->toString() + ")");
		expr = new TypeCastExpr(new Type(Type::INT), expr);
	}
	lt = scope->lalloc();
	lf = scope->lalloc();
	printf("LAB L%d\n", lt);
	reg = expr->toInst(scope, warning, error);
	printf("JMPZ VR(%d)@ L%d\n", reg, lf);
	scope->free(reg);
	stmt->toInst(scope, warning, error);
	printf("JMP L%d\n", lt);
	printf("LAB L%d\n", lf);
	return 0;
}

DoWhileStmt::DoWhileStmt(Expr *_expr, Stmt *_stmt)
	: WhileStmt(_expr, _stmt)
{
}

string DoWhileStmt::toString()
{
	return "do\n" + stmt->toString() + "\nwhile(" + expr->toString() + ");";
}

int DoWhileStmt::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	AbstractType *type = expr->getType(scope, warning, error);
	int reg, lt, lf;
	if(type->indexed) error(pos, "Type error", expr->toString() + " is an array");
	if(type->func()) error(pos, "Type error", expr->toString() + " is a function");
	if(type->type->type == Type::FLOAT)
	{
		warning(pos, "Implicit type casting", string("while(") + expr->toString() + ")");
		expr = new TypeCastExpr(new Type(Type::INT), expr);
	}
	lt = scope->lalloc();
	lf = scope->lalloc();
	printf("LAB L%d\n", lt);
	stmt->toInst(scope, warning, error);
	reg = expr->toInst(scope, warning, error);
	printf("JMPZ VR(%d)@ L%d\n", reg, lf);
	scope->free(reg);
	printf("JMP L%d\n", lt);
	printf("LAB L%d\n", lf);
	return 0;
}

ForStmt::ForStmt(Assign *_first, Expr *_second, Assign *_third, Stmt *_stmt)
	: Stmt(_first->name->pos), first(_first), second(_second), third(_third), stmt(_stmt)
{
}

string ForStmt::toString()
{
	return "for(" + first->toString() + ";" + second->toString() + ";" + third->toString() + ")\n" + stmt->toString();
}

int ForStmt::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	AbstractType *type;
	int reg, lt, lf;
	first->toInst(scope, warning, error);
	type = second->getType(scope, warning, error);
	if(type->indexed) error(pos, "Type error", second->toString() + " is an array");
	if(type->func()) error(pos, "Type error", second->toString() + " is a function");
	if(type->type->type == Type::FLOAT)
	{
		warning(pos, "Implicit type casting", string("for(") + first->toString() + ";" + second->toString() + ";" + third->toString() + ")");
		second = new TypeCastExpr(new Type(Type::INT), second);
	}
	lt = scope->lalloc();
	lf = scope->lalloc();
	printf("LAB L%d\n", lt);
	reg = second->toInst(scope, warning, error);
	printf("JMPZ VR(%d)@ L%d\n", reg, lf);
	scope->free(reg);
	stmt->toInst(scope, warning, error);
	third->toInst(scope, warning, error);
	printf("JMP L%d\n", lt);
	printf("LAB L%d\n", lf);
	return 0;
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

int IfStmt::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	AbstractType *type;
	int reg, lt, lf;
	type = expr->getType(scope, warning, error);
	if(type->indexed) error(pos, "Type error", expr->toString() + " is an array");
	if(type->func()) error(pos, "Type error", expr->toString() + " is a function");
	if(type->type->type == Type::FLOAT)
	{
		warning(pos, "Implicit type casting", "if(" + expr->toString() + ")");
		expr = new TypeCastExpr(new Type(Type::INT), expr);
	}
	reg = expr->toInst(scope, warning, error);
	lt = scope->lalloc();
	lf = scope->lalloc();
	printf("JMPZ VR(%d)@ L%d\n", reg, lf);
	scope->free(reg);
	than->toInst(scope, warning, error);
	printf("JMP L%d\n", lt);
	printf("LAB L%d\n", lf);
	if(_else)
	{
		_else->toInst(scope, warning, error);
	}
	printf("LAB L%d\n", lt);
	return 0;
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

int Case::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	for(auto it = stmt.begin(); it != stmt.end(); it++)
	{
		(*it)->toInst(scope, warning, error);
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

int SwitchStmt::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	int reg, val, breakto, ln;
	queue<int> que;
	AbstractType *type = expr->getType(scope, warning, error);
	if(type->indexed) error(pos, "Type error", expr->toString() + " is an array");
	if(type->func()) error(pos, "Type error", expr->toString() + " is a function");
	if(type->type->type == Type::FLOAT)
	{
		warning(pos, "Implicit type casting", "switch(" + expr->toString() + ")");
		expr = new TypeCastExpr(new Type(Type::INT), expr);
	}
	reg = expr->toInst(scope, warning, error);
	val = scope->ralloc();
	breakto = scope->lalloc();
	for(auto it = _case.begin(); it != _case.end(); it++)
	{
		ln = scope->lalloc();
		que.push(ln);
		if(!(*it)->isDefault())
		{
			printf("SUB VR(%d)@ %d VR(%d)\n", reg, (*it)->index->num, val);
			printf("JMPZ VR(%d)@ L%d\n", val, ln);
		}
		else
		{
			printf("JMP L%d\n", ln);
		}
	}
	scope->free(val);
	scope->free(reg);
	printf("JMP L%d\n", breakto);
	for(auto it = _case.begin(); it != _case.end(); it++)
	{
		printf("LAB L%d\n", que.front());
		que.pop();
		(*it)->toInst(scope, warning, error);
		if((*it)->_break)
		{
			printf("JMP L%d\n", breakto);
		}
	}
	printf("LAB L%d\n", breakto);
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

int CompoundStmt::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	scope = new Scope(scope, scope->func);
	for(auto it = declaration.begin(); it != declaration.end(); it++)
	{
		(*it)->toInst(scope, warning, error);
	}
	for(auto it = stmt.begin(); it != stmt.end(); it++)
	{
		(*it)->toInst(scope, warning, error);
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

int Function::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	AbstractFunctionType *func = new AbstractFunctionType(type);

	for(auto it = parameter.begin(); it != parameter.end(); it++)
	{
		func->parameter.push_back(new AbstractType((*it)->type, (*it)->identifier->indexed()));
	}
	scope->symbol.push_back(make_pair(func, name));
	scope = new Scope(scope, func);

	printf("LAB F%s\n", name->symbol.c_str());
	printf("ADD SP@ 1 SP\n");
	printf("MOVE FP@ MEM(SP@)\n");
	printf("MOVE SP@ FP\n");
	for(auto it = parameter.begin(); it != parameter.end(); it++)
	{
		(*it)->toInst(scope, warning, error);
	}
	scope->size = 0;
	compoundStmt->toInst(scope, warning, error);
	printf("MOVE FP@ SP\n");
	printf("MOVE MEM(SP@)@ FP\n");
	printf("SUB SP@ 1 SP\n");
	printf("JMP MEM(SP@)@\n");
	return 0;
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

int Program::toInst(Scope *scope, function<void(int, string, string)> warning, function<void(int, string, string)> error)
{
	scope = new Scope(scope, scope->func);
	printf("AREA SP\n");
	printf("AREA FP\n");
	printf("AREA VR\n");
	printf("AREA MEM\n");
	printf("LAB START\n");
	printf("MOVE 0 FP\n");
	printf("MOVE 0 SP\n");
	scope->local = false;
	for(auto it = declaration.begin(); it != declaration.end(); it++)
	{
		(*it)->toInst(scope, warning, error);
	}
	printf("ADD SP@ 1 SP\n");
	printf("MOVE END MEM(SP@)\n");
	printf("JMP Fmain\n");
	printf("LAB END\n");
	for(auto it = func.begin(); it != func.end(); it++)
	{
		(*it)->toInst(scope, warning, error);
	}
	printf("LAB Fprintf\n");
	printf("WRITE MEM(SP@)(-1)@\n");
	printf("JMP MEM(SP@)@\n");
	printf("LAB Fscanfi\n");
	printf("READI MEM(SP@)(-1)@\n");
	printf("JMP MEM(SP@)@\n");
	printf("LAB Fscanff\n");
	printf("READF MEM(SP@)(-1)@\n");
	printf("JMP MEM(SP@)@\n");
	return 0;
}

Scope::Scope(Scope *_parent, AbstractFunctionType *_func)
	: parent(_parent), func(_func), local(true), size(0), label(0)
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

int Scope::ralloc()
{
	int reg;
	for(reg = 0; regs.find(reg) != regs.end(); reg++);
	regs.insert(reg);
	printf("// allocating %d\n", reg);
	return reg;
}

int Scope::lalloc()
{
	if(parent)
	{
		return parent->lalloc();
	}
	return label++;
}

void Scope::free(int reg)
{
	printf("// freeing %d\n", reg);
	regs.erase(reg);
}

void Scope::addVar(Symbol *_symbol, int loc)
{
	vars[_symbol->symbol] = loc;
}

bool Scope::isLocal(Symbol *_symbol)
{
	if(vars.find(_symbol->symbol) != vars.end())
	{
		return local;
	}
	return parent->isLocal(_symbol);
}

int Scope::getVar(Symbol *_symbol)
{
	if(vars.find(_symbol->symbol) != vars.end())
	{
		return vars[_symbol->symbol];
	}
	return parent->getVar(_symbol);
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
