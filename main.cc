#include <string>
#include <iostream>
#include <stdlib.h>

#include "Parser.ih"
#include "ast.h"

using namespace std;
using namespace std::placeholders;
using namespace ast;

Program *Program::program;

void warning(int line, string message, string code)
{
	fprintf(stderr, "line %d: warning: %s\n", line, message.c_str());
	fprintf(stderr, "\t%s\n", code.c_str());
}

void error(int line, string message, string code)
{
	fprintf(stderr, "line %d: error: %s\n", line, message.c_str());
	fprintf(stderr, "\t%s\n", code.c_str());
	exit(1);
}

int main()
{
	int t;
	Parser parser;
	if(parser.parse() == 0)
	{
		Program::program->toInst(new Scope(NULL, NULL), bind(&warning, _1, _2, _3), bind(&error, _1, _2, _3));
	}

	return 0;
}
