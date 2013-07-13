#include <iostream>
#include "node.hpp"
#include "codeexec.hpp"
extern NBlock* programBlock;
extern int yyparse();

using namespace std;

int main(int argc, char *argv[]) { 
    DEBUG_PRINT(("Empieza el main.\n"));
    extern FILE *yyin;
    ++argv; --argc;
    yyin = fopen( argv[0], "r" );
    yyparse ();
    DEBUG_PRINT(("Termina yyparse();\n"));

    /*
        Se crea un contexto. Y se corre el bloque principal.
    */
    CodeExecutionContext context;
    context.executeCode(*programBlock);

    return 0;
}