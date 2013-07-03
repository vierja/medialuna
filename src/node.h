#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>

/* plagio de http://gnuu.org/2009/09/18/writing-your-own-toy-compiler/4/ */

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;

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
};

/*
    Valor Nil.
    Se podria usar un enum o algo ya que no tiene valor.
    Pero ta.
    Es lo que hay.
*/
class NNil : public NExpression {
public:
    NInteger() : { }
};

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
    NString(std::string value) : value(value) { }
};

class NIdentifier : public NExpression {
public:
    std::string name;
    NIdentifier(const std::string& name) : name(name) { }
};

class NVariable : public NExpression {

};

/*
    Variable simple. Solo un identificador.
*/
class NSimpleVariable : public NVariable {
    const NIdentifier& id;
    NSimpleVariable(const NIdentifier& id) :
        id(id) { }
};

/*
    Variable de table. Un identificador.
    TODO: 
*/

class NTableVariable : public NVariable {
    const NIdentifier&
};


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
    NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
        lhs(lhs), rhs(rhs), op(op) { }
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
        idList(idList), expresionList(expresionList) isLocal(isLocal) { }
};

/*
    Bloque general.
    Tiene una lista de statements y un lastStatment (opcional)

*/
class NBlock : public NExpression {
public:
    StatementList statements;
    NLastStatement& lastStatement;
    NBlock(): {} /* constructor sin lastStatment */
    NBlock(lastStatement) : /* constructor con lastStatment */
        lastStatement(lastStatement) { }
};

class NLastStatement : public NStatement {
public:
    ExpressionList returnList;
    int isBreak;

    NLastStatement(isBreak):
        isBreak(isBreak) { }

    NLastStatement(ExpressionList& returnList):
        isBreak(false), returnList(returnList) { }
};

class NExpressionStatement : public NStatement {
public:
    NExpression& expression;
    NExpressionStatement(NExpression& expression) :
        expression(expression) { }
};

class NMultiVariableDeclaration : public NStatement {
public:
    IdentifierList idList;
    int isLocal;
    NMultiVariableDeclaration(int isLocal):
        isLocal(isLocal) { }
    NMultiVariableDeclaration(IdentifierList idList):
        idList(idList) { }

};

class NVariableDeclaration : public NStatement {
public:
    NIdentifier& id;
    NExpression *assignmentExpr;
    NVariableDeclaration(NIdentifier& id) :
        id(id) { }
    NVariableDeclaration(NIdentifier& id, NExpression *assignmentExpr) :
        id(id), assignmentExpr(assignmentExpr) { }
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
    NForLoop(IdentifierList nameList, ExpressionList expresionList) :
        nameList(nameList), expresionList(expresionList) { }
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
    NForLoop(NIdentifier& id, ExpressionList expresionList) :
        id(id), expresionList(expresionList) { }
};

/*
    * ignorando variable someFunction
    someFunction = function(parameter, anotherParameter) print("hola") end
    /---------/
    arguments = ["parameter", "anotherParameter"]
    block = ```print("hola")```

*/
class NAnonFunctionDeclaration : public NStatement {
public:
    VariableList arguments;
    NBlock& block;
    NFunctionDeclaration(VariableList& arguments, NBlock& block) :
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
    VariableList arguments;
    NBlock& block;
    NFunctionDeclaration(NIdentifier& id, VariableList& arguments, NBlock& block) :
        id(id), arguments(arguments), block(block) { }
};

#endif NODE_H
