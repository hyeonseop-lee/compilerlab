#include <string>
#include <iostream>

#include "Parser.ih"
#include "ast.h"

using namespace std;
using namespace ast;

Program *Program::program;

int main()
{
	Parser parser;
	parser.parse();

	return 0;
}
