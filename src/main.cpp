#include <iostream>
#include "node.h"
extern NBlock* programBlock;
extern int yyparse();

int main(int argc, char **argv)
{
    std::cout << "Empieza el main." << std::endl;
    yyparse();
    std::cout << "Termina yyparse();" << std::endl;
    std::cout << programBlock << std::endl;
    StatementList::iterator it;
    for (it = programBlock->statements.begin(); it != programBlock->statements.end(); it++){
        std::cout << "Statement: " << (**it).str_type() << std::endl;
    }
    return 0;
}