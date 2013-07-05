#include "node.h"
#include "codeexec.h"
#include "parser.hpp" /* para tener acceso al enum de tokens */

using namespace std;

void* CodeExecutionContext::executeCode(NBlock& root){
    /*
        Se crea un nuevo CodeExecutionBlock y en el top del stack blocks.
    */
    CodeExecutionBlock* execBlock = new CodeExecutionBlock(root);
    this->blocks.push(execBlock);

    /*
        Run
    */
    cout << "Se corre el codigo. Size del stack: " << this->blocks.size() << endl;
    /*
        Un bloque esta compuesto de una lista de statements y un last statement.
    */
    root.runCode(*this);

    cout << "Se corrio el codigo" << endl;
    this->blocks.pop();
}