bison -d -o parser.cpp parser.y
lex -o scanner.cpp scanner.lex
g++ -o parser parser.cpp scanner.cpp main.cpp