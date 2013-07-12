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

test_files := $(wildcard ../test_*)

test: medialuna test_compare test_if test_for_loop test_print_expressions test_simple_functions test_math test_table

test_compare:
	./medialuna ../test/test_compare.lua > test_compare_medialuna.txt
	lua ../test/test_compare.lua > test_compare_lua.txt
	diff test_compare_medialuna.txt test_compare_lua.txt
	rm test_compare_medialuna.txt test_compare_lua.txt

test_if:
	./medialuna ../test/test_if.lua > test_if_medialuna.txt
	lua ../test/test_if.lua > test_if_lua.txt
	diff test_if_medialuna.txt test_if_lua.txt
	rm test_if_medialuna.txt test_if_lua.txt

test_for_loop:
	./medialuna ../test/test_for_loop.lua > test_for_loop_medialuna.txt
	lua ../test/test_for_loop.lua > test_for_loop_lua.txt
	diff test_for_loop_medialuna.txt test_for_loop_lua.txt
	rm test_for_loop_medialuna.txt test_for_loop_lua.txt

test_print_expressions:
	./medialuna ../test/test_print_expressions.lua > test_print_expressions_medialuna.txt
	lua ../test/test_print_expressions.lua > test_print_expressions_lua.txt
	diff test_print_expressions_medialuna.txt test_print_expressions_lua.txt
	rm test_print_expressions_medialuna.txt test_print_expressions_lua.txt

test_simple_functions:
	./medialuna ../test/test_simple_functions.lua > test_simple_functions_medialuna.txt
	lua ../test/test_simple_functions.lua > test_simple_functions_lua.txt
	diff test_simple_functions_medialuna.txt test_simple_functions_lua.txt
	rm test_simple_functions_medialuna.txt test_simple_functions_lua.txt

test_math:
	./medialuna ../test/test_math.lua > test_math_medialuna.txt
	lua ../test/test_math.lua > test_math_lua.txt
	diff test_math_medialuna.txt test_math_lua.txt
	rm test_math_medialuna.txt test_math_lua.txt

test_table:
	./medialuna ../test/test_table.lua > test_table_medialuna.txt
	lua ../test/test_table.lua > test_table_lua.txt
	diff test_table_medialuna.txt test_table_lua.txt
	rm test_table_medialuna.txt test_table_lua.txt
