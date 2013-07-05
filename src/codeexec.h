#ifndef CODEEXEC_H
#define CODEEXEC_H

/*#include <stack>

class NBlock;
class NExpression;

class CodeExecutionBlock {
public:
    CodeExecutionBlock();
    CodeExecutionBlock(NBlock* block): block(block) {};
    NBlock* block;
    //TODO: Pensar como guardar las variables y funciones declaradas localmente.
    std::map<std::string, NExpression*> locals;
    std::map<std::string, NFunctionDeclaration*> localFunctions;
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
    *
    void* executeCode(NBlock& root); // Puede devolver cualquier cosa.
};*/

#endif