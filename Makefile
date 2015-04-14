all:
	flexc++ lexer
	bisonc++ grammar
	g++ --std=c++11 *.cc -o parser
