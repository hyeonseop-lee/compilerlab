# compilerlab

Compiler Design Labs in KAIST CS420, 2015 Spring

### Building Scanner&Parser
Required:
- Flexc++
- Bisonc++

Installation on Ubuntu:
```sh
sudo apt-get install flexc++ bisonc++
```

Building:
```sh
flexc++ lexer
bisonc++ grammar
```

### Building Application
Required:
- g++ with C++11

Installation on Ubuntu:
```sh
sudo apt-get install g++
```

Building:
```sh
g++ --std=c++11 *.cc -o parser
```

### Running Application
```sh
./parser
```
- Standard Input: source code
- Standard Output: AST from given source code
- Standard Error: Symnol table from given source code
