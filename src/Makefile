all: interpreter

clean:
	rm parser.cpp parser.hpp parser scanner.cpp *.o

parser.cpp: parser.y
	bison -d -o $@ $^

parser.hpp: parser.cpp

scanner.cpp: scanner.lex parser.hpp
	lex -o $@ $^

parser: parser.cpp codeexec.cpp main.cpp scanner.cpp
	g++ -o $@ *.cpp