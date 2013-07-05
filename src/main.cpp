#include <iostream>
#include "node.h"
extern NBlock* programBlock;
extern int yyparse();

using namespace std;

int main(int argc, char **argv)
{
    cout << "Empieza el main." << endl;
    yyparse();
    cout << "Termina yyparse();" << endl;
    cout << "ProgramBlock: " << programBlock << endl;
    /*
        Se crea un contexto. Y se corre el bloque principal.
    */
    //CodeExecutionContext context;
    /*
        TODO:
        Falta inicializar el contexto con las funciones basicas de table y print.
    */
    //context.executeCode(*programBlock);


    StatementList::iterator it;
    /* Recorrero los statements del programBlock */
    for (it = programBlock->statements.begin(); it != programBlock->statements.end(); it++){
        /* Chequeo que tipo es. Enum definido en node.h */
        if ((**it).type() == FUNCTION_DECLARATION){
            /* Hago el dynamic_cast para poder usarlo. */
            NFunctionDeclaration* functionDeclaration = dynamic_cast<NFunctionDeclaration*>(*it);
            
            cout << "Function declaration" << endl;
            IdentifierList::iterator it2;
            /* Recorro los identificadores.*/
            int counter = 0;
            for (it2 = functionDeclaration->arguments.begin(); it2 != functionDeclaration->arguments.end(); it2++){
                NIdentifier* identifier = dynamic_cast<NIdentifier*>(*it2);
                cout << "Argument numero " << counter << ": " << identifier->name << endl;
                counter++;
            }
        }
    }
    return 0;
}