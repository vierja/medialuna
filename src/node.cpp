#include "node.h"
#include "codeexec.h"

using namespace std;

void NBlock::runCode(CodeExecutionContext& context) {
    cout << "NBlock::runCode" << endl;
    StatementList::const_iterator it;
    for (it = statements.begin(); it != statements.end(); it++){
        cout << "Running code for " << typeid(**it).name() << endl;
        (**it).runCode(context);
    }
    
    return lastStatement.runCode(context);

    // Borro las variables y funciones locales del contexto.
    //context.localVariables.clear();
    //context.localFunctions.clear();
}

void NFunctionDeclaration::runCode(CodeExecutionContext& context) {
    /*
        Cuando se declara una funcion no se corre, pero se debe guardar 
        en el contexto.
        ¿Como se guarda?

        NIdentifier& id;
        IdentifierList arguments;
        NBlock& block;
    */
    string funcName = id.name;
    cout << "Se declara la funcion " << funcName << endl;
    //context.localFunctions.add(funcName, *this);
}

void NMultiAssignment::runCode(CodeExecutionContext& context) {
    /*
        Cuando se tiene multi assignment hay varias cosas para tener
        en cuenta.
        Si es local entonces hay que ver como tener prioridad ante el resto.
    */
}

void NExpressionStatement::runCode(CodeExecutionContext& context) {

}


void NLastStatement::runCode(CodeExecutionContext& context) {

}

void NForLoopIn::runCode(CodeExecutionContext& context) {

}

void NForLoopAssign::runCode(CodeExecutionContext& context) {

}

void NMultiVariableDeclaration::runCode(CodeExecutionContext& context) {

}