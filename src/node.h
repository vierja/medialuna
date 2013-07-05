#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>

/* plagio modificado de http://gnuu.org/2009/09/18/writing-your-own-toy-compiler/4/ */

enum NodeType {
    MULTI_ASSIGNMENT,
    LAST_STATMENT,
    EXPRESSION_STATMENT,
    MULTI_VARIABLE_DECLARATION,
    VARIABLE_DECLARATION,
    FOR_LOOP_IN,
    FOR_LOOP_ASSIGN,
    FUNCTION_DECLARATION
};

//class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;
class NIdentifier;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;
typedef std::vector<NIdentifier*> IdentifierList;

class Node {
public:
    virtual ~Node() {}
};

class NExpression : public Node {
};

class NStatement : public Node {
public:
    virtual NodeType type() = 0;
};

/*
    Valor Nil.
    Se podria usar un enum o algo ya que no tiene valor.
    Pero ta.
    Es lo que hay.
*/
class NNil : public NExpression {
public:
    NNil() { }
};

/*
    Valor Booleano
    trueVal = 1 si es True
              0 si es False
*/
class NBoolean : public NExpression {
public:
    int trueVal;
    NBoolean(int trueVal) : trueVal(trueVal) { }
};

class NInteger : public NExpression {
public:
    long long value;
    NInteger(long long value) : value(value) { }
};

class NDouble : public NExpression {
public:
    double value;
    NDouble(double value) : value(value) { }
};

class NString : public NExpression {
public:
    std::string value;
    NString(const std::string& value) : value(value) { }
};

class NIdentifier : public NExpression {
public:
    std::string name;
    NIdentifier(const std::string& name) : name(name) { }
};

/*
    TODO:
    Esto en verdad teoricamente puede recibir una lista
    de identificadores secuenciales separados por un .
    Para simplificar solo permitimos un nivel.
    Es para hacer:
        table.add()
    Donde id    = 'add'
          library = 'table'


class NIdentifierMethod : public NIdentifier {
public:
    const NIdentifier& id;
    const NIdentifier& library;
    NIdentifierMethod(const NIdentifier& id, const NIdentifier& library) :
        id(id), library(library) { }
};*/

class NFunctionCall : public NExpression {
public:
    const NIdentifier& id;
    ExpressionList arguments;
    NFunctionCall(const NIdentifier& id, ExpressionList& arguments) :
        id(id), arguments(arguments) { }
    NFunctionCall(const NIdentifier& id) : id(id) { }
};

class NBinaryOperator : public NExpression {
public:
    int op;
    NExpression& lhs;
    NExpression& rhs;
    NBinaryOperator(int op, NExpression& lhs, NExpression& rhs) :
        op(op), lhs(lhs), rhs(rhs) { }
};

class NUnaryOperator : public NExpression {
public:
    int op;
    NExpression& rhs;
    NUnaryOperator(NExpression& rhs, int op) :
        rhs(rhs), op(op) { }
};

/*
    Multiple asignacion (y simple)

    A, B, C, D = "a", "b", "oops I took both c and d"

    idList = ["A", "B", "C", "D"]
    expresionList = ["a", "b", "oops I took both C and D"]

    Creo que idList.length() >= expresionList.length()
*/
class NMultiAssignment : public NStatement {
public:
    IdentifierList idList;
    ExpressionList expresionList;
    int isLocal;
    NMultiAssignment(IdentifierList idList, ExpressionList expresionList, int isLocal) :
        idList(idList), expresionList(expresionList), isLocal(isLocal) { }
    NodeType type(){ return MULTI_ASSIGNMENT; }
};

class NLastStatement : public NStatement {
public:
    ExpressionList returnList;
    int isBreak;

    NLastStatement(int isBreak, ExpressionList& returnList):
        isBreak(isBreak), returnList(returnList) { }
    NodeType type(){ return LAST_STATMENT; }
};

/*
    Bloque general.
    Tiene una lista de statements y un lastStatment (opcional)

*/
class NBlock : public NExpression {
public:
    StatementList statements;
    NLastStatement& lastStatement;
    NBlock(NLastStatement& lastStatement) : /* constructor con lastStatment */
        lastStatement(lastStatement) { }

    void statements_add_list(StatementList& statements){
        StatementList::const_iterator it;
        for (it = statements.begin(); it != statements.end(); it++){
            this->statements.push_back(*it);
        }
    };
};

class NExpressionStatement : public NStatement {
public:
    NExpression& expression;
    NExpressionStatement(NExpression& expression) :
        expression(expression) { }
    NodeType type(){ return EXPRESSION_STATMENT; }
};

class NMultiVariableDeclaration : public NStatement {
public:
    IdentifierList idList;
    int isLocal;
    NMultiVariableDeclaration(int isLocal, IdentifierList& idList):
        isLocal(isLocal), idList(idList) { }
    NodeType type(){ return MULTI_VARIABLE_DECLARATION; }

};

class NVariableDeclaration : public NStatement {
public:
    NIdentifier& id;
    NExpression *assignmentExpr;
    NVariableDeclaration(NIdentifier& id) :
        id(id) { }
    NVariableDeclaration(NIdentifier& id, NExpression *assignmentExpr) :
        id(id), assignmentExpr(assignmentExpr) { }
    NodeType type(){ return VARIABLE_DECLARATION; }
};

/*
    forloop usando in

    for key, value in pairs(t) do print(key, value) end

    nameList = ["key", "value"]
    expresionList = ["pairs(t)"]
    body = '''print(key, value)'''
*/
class NForLoopIn : public NStatement {
public:
    IdentifierList nameList;
    ExpressionList expresionList;
    NBlock& block;
    NForLoopIn(IdentifierList nameList, ExpressionList expresionList, NBlock& block) :
        nameList(nameList), expresionList(expresionList), block(block) { }
    NodeType type(){ return FOR_LOOP_IN; }
}; 

/*
    forloop con asignacion y limite e incremento definidos.

    for the_number = 1, MAXIMUM, STEP do print(the_number) end

    id = "the_number"
    expresionList = ["1", "MAXIMUM", "STEP"]
    body = ```print(the_number)```

    expresionList es de largo 2 o 3.
*/
class NForLoopAssign : public NStatement {
public:
    NIdentifier& id;
    ExpressionList expresionList;
    NBlock& block;
    NForLoopAssign(NIdentifier& id, ExpressionList expresionList, NBlock& block) :
        id(id), expresionList(expresionList), block(block) { }
    NodeType type(){ return FOR_LOOP_ASSIGN; }
};


/*
    * ignorando variable someFunction
    someFunction = function(parameter, anotherParameter) print("hola") end
    /---------/
    arguments = ["parameter", "anotherParameter"]
    block = ```print("hola")```

*/
class NAnonFunctionDeclaration : public NExpression {
public:
    IdentifierList arguments;
    NBlock& block;
    NAnonFunctionDeclaration(IdentifierList& arguments, NBlock& block) :
        arguments(arguments), block(block) { }
};

/*
    function someFunction(parameter, anotherParameter)
        print("hola")
    end
    /---------/
    id = 'someFunction'
    arguments = ["parameter", "anotherParameter"]
    block = ```print("hola")```
*/
class NFunctionDeclaration : public NStatement {
public:
    NIdentifier& id;
    IdentifierList arguments;
    NBlock& block;
    NFunctionDeclaration(NIdentifier& id, IdentifierList& arguments, NBlock& block) :
        id(id), arguments(arguments), block(block) { }
    NodeType type(){ return FUNCTION_DECLARATION; }
};

#endif NODE_H
