all: medialuna

clean:
	rm parser.cpp parser.hpp medialuna scanner.cpp

parser.cpp: parser.y
	bison -d -o $@ $^

parser.hpp: parser.cpp

scanner.cpp: scanner.lex parser.hpp
	lex -o $@ $^

medialuna: parser.cpp codeexec.cpp main.cpp scanner.cpp node.cpp
	g++ -o $@ *.cpp