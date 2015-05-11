#include <string>
#include <iostream>

#include "Parser.ih"
#include "ast.h"

using namespace std;
using namespace ast;

Program *Program::program;

int main()
{
	int t;
	Traversal traversal;
	Parser parser;
	if(parser.parse() == 0)
	{
		cout << Program::program->toString() << endl;
		Program::program->traverse(&traversal, 0, NULL);
		for(auto it = traversal.level.begin(); it != traversal.level.end(); it++)
		{
			for(auto jt = it->second.begin(); jt != it->second.end(); )
			{
				if((*jt)->entry.empty())
				{
					delete *jt;
					jt = it->second.erase(jt);
				}
				else
				{
					jt++;
				}
			}
			t = 'a';
			for(auto jt = it->second.begin(); jt != it->second.end(); jt++, t++)
			{
				for(auto kt = (*jt)->entry.begin(); kt != (*jt)->entry.end(); kt++)
				{
					if(it->second.size() == 1)
					{
						fprintf(stderr, "<%s, <%s, %d, %s>>\n", kt->name.c_str(), kt->type.c_str(), it->first, kt->location.c_str());
					}
					else
					{
						fprintf(stderr, "<%s, <%s, %d%c, %s>>\n", kt->name.c_str(), kt->type.c_str(), it->first, t, kt->location.c_str());
					}
				}
			}
		}
	}

	return 0;
}
