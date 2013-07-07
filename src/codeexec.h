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
    map<string, NFunctionDeclaration*> functions;
};

class CodeExecutionContext {
public:
    // La lista de blocks funciona como un stack, pero permite facilmente recorrerla.
    vector<CodeExecutionBlock*> blocks;

    /**
     * executeCode toma como parametro un NBlock y ejecuta el codigo.
     * para esto debe tomar en cuenta el stack de bloques (el contexto).
     *
     * inicialmente cuando se corre se le pasa el bloque principal y
     * el stack se encuentra vacio.
    */
    void* executeCode(NBlock& root); // Puede devolver cualquier cosa.

    void push_block(CodeExecutionBlock* ceb);
    void pop_block();
    /*
        Devuelve la variable con el nombre `name` en el contexto.
        Empieza a buscar desde el ultimo al primero.
    */
    NExpression* getVariable(string name);

    /*
        Devuelve la funcion (NFunctionDeclaration) con el nombre `name`
        en el contexto.
        Empieza a buscar desde el ultimo al primero.
    */
    NFunctionDeclaration* getFunction(string name);

    /*
    *   Agrega o sobreescribe la funcion de nombre `name` al contexto.
    *   Se guarda en el mapa `functions` del ultimo bloque.
    */
    void addFunction(string name, NFunctionDeclaration* function);

    /*
        Agrega o sobreescribe la variable de nombre `name` al contexto.
        Si la variable es local (`isLocal` == 1) entonces se guarda
        en el ultimo bloque.
        Si no es local (`isLocal` == 0) entonces se busca en el contexto
        y se sobreescribe. Si no se encuentra se agrega en el ultimo bloque.
    */
    void addVariable(string name, NExpression* expression, int isLocal);

    /*
        Chequea si una funcion de nombre `name` se encuentra definida en el contexto.
    */
    int functionDefined(string name);

    /*
        Analogo al `print` de Lua.
    */
    void print(ExpressionList exprList);

    /*
        Aux al `print` de Lua. Imprime una expression.
    */
    void print_expr(NExpression* expr);

    /*
        Para debuggear.
        Imprime las variables y funciones del contexto recorriendo los bloques
        del ultimo hacia el primero.
    */
    void printFunctionsAndVariables();
};

#endif