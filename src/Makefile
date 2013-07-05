all: parser

clean:
	rm parser.cpp parser.hpp parser scanner.cpp

parser.cpp: parser.y
	bison -d -o $@ $^

parser.hpp: parser.cpp

scanner.cpp: scanner.lex parser.hpp
	lex -o $@ $^

parser: parser.cpp codeexec.cpp main.cpp scanner.cpp node.cpp
	g++ -o $@ *.cpp