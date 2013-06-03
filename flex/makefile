LDFLAGS=-ll
LEX=flex
EXE=lua_lex

all: $(EXE)

$(EXE): $(EXE).o
	$(CC) -o $@  $(LDFLAGS) $^

$(EXE).o: $(EXE).c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $^

$(EXE).c: $(EXE).l
	$(LEX) $(LFLAGS) -o $@ $^

clean:
	$(RM) *.o $(EXE).c

test1:
	@./$(EXE) ../test/test1.lua

test2:
	@./$(EXE) ../test/test2.lua


test: test1 test2