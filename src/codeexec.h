#ifndef CODEEXEC_H
#define CODEEXEC_H

#include <stack>
#include <map>

using namespace std;

class NBlock;
class NExpression;
class NFunctionDeclaration;

class CodeExecutionBlock {
public:
    CodeExecutionBlock();
    CodeExecutionBlock(NBlock& block) : block(block) {};
    NBlock& block;

    /*
    Para que sirve tener variables y localVariables (analogo para functions).
    Cuando en un statement se quiere utilizar una variable esta se busca primero
    en las variables locales y si no encuentra se busca en las variables (que pueden
    se compartidas entre distintos bloques y no se borran del contexto general del programa.)

    Hay que tener cuidado porque creo que cuando en un bloque se usa una variable pero no se
    pone local entonces si no existe ya es igual a una local.
    */
    map<string, NExpression*> variables;
    map<string, NExpression*> localVariables;
    map<string, NFunctionDeclaration*> functions;
    map<string, NFunctionDeclaration*> localFunctions;
};

class CodeExecutionContext {
public:
    std::stack<CodeExecutionBlock*> blocks;

    /**
     * executeCode toma como parametro un NBlock y ejecuta el codigo.
     * para esto debe tomar en cuenta el stack de bloques (el contexto).
     *
     * inicialmente cuando se corre se le pasa el bloque principal y
     * el stack se encuentra vacio.
    */
    void* executeCode(NBlock& root); // Puede devolver cualquier cosa.
};

#endif