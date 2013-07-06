#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>

/* plagio modificado de http://gnuu.org/2009/09/18/writing-your-own-toy-compiler/4/ */

enum NodeType {
    MULTI_ASSIGNMENT,
    LAST_STATMENT,
    EXPRESSION_STATMENT,
    //MULTI_VARIABLE_DECLARATION,
    VARIABLE_DECLARATION,
    FOR_LOOP_IN,
    FOR_LOOP_ASSIGN,
    FUNCTION_DECLARATION,
    NIL,
    BOOLEAN,
    INTEGER,
    DOUBLE,
    STRING,
    IDENTIFIER,
    FUNCTION_CALL,
    BINARY_OPERATOR,
    UNARY_OPERATOR,
    BLOCK,
    ANON_FUNCTION_DECLARATION,
    EXPRESSION
};

class CodeExecutionContext;
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
public:
    virtual NodeType type() { return EXPRESSION; };
    virtual NExpression* evaluate(CodeExecutionContext& context) = 0;
};

class NStatement : public Node {
public:
    virtual NodeType type() = 0;
    virtual void runCode(CodeExecutionContext& context) = 0;
};

/*
    TODO: Ignorar hasta demostrar que se puede usar un subset de NExpression como valor de retorno.
    Valor de retorno de una funcion, no se usa en el parsing
    pero si se utiliza en la ejecucion del codigo.
/*
/*

class NReturnValue : public Node {

};

/*
    No retorna nada.

class NReturnNil : public NReturnValue {
public:
    NReturnValue(){}
};

class NReturnBoolean : public NReturnValue {
public:
    int trueVal;
    NReturnValue(int trueVal): trueVal(trueVal) { }
};
class NReturnInteger : public NReturnValue {
public:

};

*/



/*
    Valor Nil.
    Se podria usar un enum o algo ya que no tiene valor.
    Pero ta.
    Es lo que hay.
*/
class NNil : public NExpression {
public:
    NNil() { }
    NodeType type(){ return NIL; }
    NExpression* evaluate(CodeExecutionContext& context){ return this; };
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
    NodeType type(){ return BOOLEAN; }
    NExpression* evaluate(CodeExecutionContext& context){ return this; }
};

class NInteger : public NExpression {
public:
    long long value;
    NInteger(long long value) : value(value) { }
    NodeType type(){ return INTEGER; }
    NExpression* evaluate(CodeExecutionContext& context){ return this; };
};

class NDouble : public NExpression {
public:
    double value;
    NDouble(double value) : value(value) { }
    NodeType type(){ return DOUBLE; }
    NExpression* evaluate(CodeExecutionContext& context){ return this; };
};

class NString : public NExpression {
public:
    std::string value;
    NString(const std::string& value) : value(value) { }
    NodeType type(){ return STRING; }
    NExpression* evaluate(CodeExecutionContext& context){ return this; };
};

class NIdentifier : public NExpression {
public:
    std::string name;
    NIdentifier(const std::string& name) : name(name) { }
    NodeType type(){ return IDENTIFIER; }
    NExpression* evaluate(CodeExecutionContext& context);
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
    NIdentifier& id;
    ExpressionList arguments;
    NFunctionCall(NIdentifier& id, ExpressionList& arguments) :
        id(id), arguments(arguments) { }
    NFunctionCall(NIdentifier& id) : id(id) { }
    NodeType type(){ return FUNCTION_CALL; }
    NExpression* evaluate(CodeExecutionContext& context);
};

class NBinaryOperator : public NExpression {
public:
    int op;
    NExpression& lhs;
    NExpression& rhs;
    NBinaryOperator(int op, NExpression& lhs, NExpression& rhs) :
        op(op), lhs(lhs), rhs(rhs) { }
    NodeType type(){ return BINARY_OPERATOR; }
    NExpression* evaluate(CodeExecutionContext& context);
};

class NUnaryOperator : public NExpression {
public:
    int op;
    NExpression& rhs;
    NUnaryOperator(NExpression& rhs, int op) :
        rhs(rhs), op(op) { }
    NodeType type(){ return UNARY_OPERATOR; }
    NExpression* evaluate(CodeExecutionContext& context);
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
    void runCode(CodeExecutionContext& context);
};

class NLastStatement : public NStatement {
public:
    ExpressionList returnList;
    int isBreak;

    NLastStatement(int isBreak, ExpressionList& returnList):
        isBreak(isBreak), returnList(returnList) { }
    NodeType type(){ return LAST_STATMENT; }
    void runCode(CodeExecutionContext& context);
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

    void runCode(CodeExecutionContext& context);
    NodeType type(){ return BLOCK; }
    NExpression* evaluate(CodeExecutionContext& context);
};

class NExpressionStatement : public NStatement {
public:
    NExpression& expression;
    NExpressionStatement(NExpression& expression) :
        expression(expression) { }
    NodeType type(){ return EXPRESSION_STATMENT; }

    void runCode(CodeExecutionContext& context);
};

/*class NMultiVariableDeclaration : public NStatement {
public:
    IdentifierList idList;
    int isLocal;
    NMultiVariableDeclaration(int isLocal, IdentifierList& idList):
        isLocal(isLocal), idList(idList) { }
    NodeType type(){ return MULTI_VARIABLE_DECLARATION; }
    void runCode(CodeExecutionContext& context);

};*/

class NVariableDeclaration : public NStatement {
public:
    NIdentifier& id;
    NExpression *assignmentExpr;
    NVariableDeclaration(NIdentifier& id) :
        id(id) { }
    NVariableDeclaration(NIdentifier& id, NExpression *assignmentExpr) :
        id(id), assignmentExpr(assignmentExpr) { }
    NodeType type(){ return VARIABLE_DECLARATION; }
    void runCode(CodeExecutionContext& context);
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
    void runCode(CodeExecutionContext& context);
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
    void runCode(CodeExecutionContext& context);
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
    NodeType type(){ return ANON_FUNCTION_DECLARATION; }
    NExpression* evaluate(CodeExecutionContext& context);
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
    void runCode(CodeExecutionContext& context);
};

#endif NODE_H
