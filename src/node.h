#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>
#include <sstream>

//#define DEBUG

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */
// Macro para imprimir mensajes de debuggeo.
#ifdef DEBUG
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x) do {} while (0)
#endif

/* plagio modificado de http://gnuu.org/2009/09/18/writing-your-own-toy-compiler/4/ */

enum NodeType {
    MULTI_ASSIGNMENT,
    IF_CONDITION,
    IF,
    LAST_STATEMENT,
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
    EXPRESSION_LIST,
    FUNCTION_CALL,
    BINARY_OPERATOR,
    UNARY_OPERATOR,
    BLOCK,
    ANON_FUNCTION_DECLARATION,
    EXPRESSION,
    BREAK,
    TABLE_FIELD_SINGLE_EXPRESSION,
    TABLE_FIELD_IDENTIFIER,
    TABLE_FIELD_EXPRESSION,
    TABLE_FIELD_LIST,
    TABLE,
    TABLE_EXPRESSION,
    TABLE_FIELD_KEY
};

class CodeExecutionContext;
class NStatement;
class NExpression;
class NVariableDeclaration;
class NIdentifier;
class NIfCond;
class NTableField;
class NTableFieldExpression;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;
typedef std::vector<NIdentifier*> IdentifierList;
typedef std::vector<NIfCond*> ConditionList;
typedef std::vector<NTableField*> GenericTableFieldList;
typedef std::vector<NTableFieldExpression*> TableFieldList;

class Node {
public:
    virtual ~Node() {}
    virtual NodeType type() = 0;
    virtual std::string type_str() = 0;
};

class NExpression : public Node {
public:
    virtual NodeType type() { return EXPRESSION; };
    virtual std::string type_str() { return "EXPRESSION"; };
    virtual NExpression* evaluate(CodeExecutionContext& context) = 0;
    virtual NExpression* clone() const = 0;
};

class NStatement : public Node {
public:
    virtual NodeType type() = 0;
    virtual NExpression* runCode(CodeExecutionContext& context) = 0;
    virtual std::string type_str() = 0;
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
    NodeType type(){ return NIL; }
    std::string type_str(){ return "NIL"; }
    NExpression* evaluate(CodeExecutionContext& context){ return this; };
    virtual NNil* clone() const { return new NNil(*this); }

};

class NBreak : public NExpression {
public:
    NBreak() { }
    NodeType type(){ return BREAK; }
    std::string type_str(){ return "BREAK"; }
    virtual NBreak* clone() const { return new NBreak(*this); }
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
    std::string type_str(){ return "BOOLEAN"; }
    virtual NBoolean* clone() const { return new NBoolean(*this); }
    NExpression* evaluate(CodeExecutionContext& context){ return this; }
};

class NInteger : public NExpression {
public:
    long long value;
    NInteger(long long value) : value(value) { }
    NodeType type(){ return INTEGER; }
    std::string type_str(){ return "INTEGER"; }
    virtual NInteger* clone() const { return new NInteger(*this); }
    NExpression* evaluate(CodeExecutionContext& context){ return this; };
};

class NDouble : public NExpression {
public:
    double value;
    NDouble(double value) : value(value) { }
    NodeType type(){ return DOUBLE; }
    std::string type_str(){ return "DOUBLE"; }
    virtual NDouble* clone() const { return new NDouble(*this); }
    NExpression* evaluate(CodeExecutionContext& context){ return this; };
};

class NString : public NExpression {
public:
    std::string value;
    NString(const std::string& value) : value(value) { }
    NodeType type(){ return STRING; }
    std::string type_str(){ return "STRING"; }
    virtual NString* clone() const { return new NString(*this); }
    NExpression* evaluate(CodeExecutionContext& context){ return this; };
};

class NIdentifier : public NExpression {
public:
    std::string name;
    NIdentifier(const std::string& name) : name(name) { }
    NodeType type(){ return IDENTIFIER; }
    std::string type_str(){ return "IDENTIFIER"; }
    virtual NIdentifier* clone() const { return new NIdentifier(*this); }
    NExpression* evaluate(CodeExecutionContext& context);
};

class NExpressionList : public NExpression {
public:
    ExpressionList exprList;
    NExpressionList(ExpressionList& exprList):
        exprList(exprList) { }
    NodeType type(){ return EXPRESSION_LIST; }
    std::string type_str(){ return "EXPRESSION_LIST"; }
    virtual NExpressionList* clone() const { return new NExpressionList(*this); }
    NExpression* evaluate(CodeExecutionContext& context);
};

class NFunctionCall : public NExpression {
public:
    NIdentifier& id;
    ExpressionList arguments;
    NFunctionCall(NIdentifier& id, ExpressionList& arguments) :
        id(id), arguments(arguments) { }
    NFunctionCall(NIdentifier& id) : id(id) { }
    NodeType type(){ return FUNCTION_CALL; }
    std::string type_str(){ return "FUNCTION_CALL"; }
    virtual NFunctionCall* clone() const { return new NFunctionCall(*this); }
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
    std::string type_str(){ return "BINARY_OPERATOR"; }
    virtual NBinaryOperator* clone() const { return new NBinaryOperator(*this); }
    NExpression* evaluate(CodeExecutionContext& context);
};

class NUnaryOperator : public NExpression {
public:
    int op;
    NExpression& rhs;
    NUnaryOperator(NExpression& rhs, int op) :
        rhs(rhs), op(op) { }
    NodeType type(){ return UNARY_OPERATOR; }
    std::string type_str(){ return "UNARY_OPERATOR"; }
    virtual NUnaryOperator* clone() const { return new NUnaryOperator(*this); }
    NExpression* evaluate(CodeExecutionContext& context);
};

class NTableField : public NExpression {
};

class NTableFieldSingleExpression : public NTableField {
public:
    NExpression& expression;
    NTableFieldSingleExpression(NExpression& expression) :
        expression(expression) {}
    NodeType type(){ return TABLE_FIELD_SINGLE_EXPRESSION; }
    std::string type_str(){ return "TABLE_FIELD_SINGLE_EXPRESSION"; }
    virtual NTableFieldSingleExpression* clone() const { return new NTableFieldSingleExpression(*this); }
    NExpression* evaluate(CodeExecutionContext& context);
};

class NTableFieldIdentifier : public NTableField {
public:
    NIdentifier& identifier;
    NExpression& expression;
    NTableFieldIdentifier(NIdentifier& identifier, NExpression& expression) :
        identifier(identifier), expression(expression) {}
    NodeType type(){ return TABLE_FIELD_IDENTIFIER; }
    std::string type_str(){ return "TABLE_FIELD_IDENTIFIER"; }
    virtual NTableFieldIdentifier* clone() const { return new NTableFieldIdentifier(*this); }
    NExpression* evaluate(CodeExecutionContext& context);
};

class NTableFieldExpression : public NTableField {
public:
    NExpression& keyExpr;
    NExpression& valExpr;
    int auto_incremented;
    NTableFieldExpression(NExpression& keyExpr, NExpression& valExpr, int auto_incremented) :
        keyExpr(keyExpr), valExpr(valExpr), auto_incremented(auto_incremented) {}
    NodeType type(){ return TABLE_FIELD_EXPRESSION; }
    std::string type_str(){ return "TABLE_FIELD_EXPRESSION"; }
    virtual NTableFieldExpression* clone() const { return new NTableFieldExpression(*this); }
    NExpression* evaluate(CodeExecutionContext& context);
};

class NTableFieldList : public NExpression {
public:
    GenericTableFieldList fieldList;
    NTableFieldList(GenericTableFieldList fieldList) :
        fieldList(fieldList) {}
    NodeType type(){ return TABLE_FIELD_LIST; }
    std::string type_str(){ return "TABLE_FIELD_LIST"; }
    virtual NTableFieldList* clone() const { return new NTableFieldList(*this); }
    NExpression* evaluate(CodeExecutionContext& context);
};

class NTableFieldKey : public NExpression {
public:
    NExpression& tableIdent;
    NExpression& keyExpr;
    NTableFieldKey(NExpression& tableIdent, NExpression& keyExpr) :
        tableIdent(tableIdent), keyExpr(keyExpr) {}
    NodeType type(){ return TABLE_FIELD_KEY; }
    std::string type_str(){ return "TABLE_FIELD_KEY"; }
    virtual NTableFieldKey* clone() const { return new NTableFieldKey(*this); }
    NExpression* evaluate(CodeExecutionContext& context){ return this; };
};

class NTable : public NExpression {
public:
    NTableFieldList& fieldList;
    NTable(NTableFieldList& fieldList) :
        fieldList(fieldList) {}
    NodeType type(){ return TABLE; }
    std::string type_str(){ return "TABLE"; }
    virtual NTable* clone() const { return new NTable(*this); }
    NExpression* evaluate(CodeExecutionContext& context);
};

class NTableExpr : public NExpression {
public:
    int counter;
    TableFieldList* fieldList;
    NTableExpr() : counter(1) {};
    NodeType type(){ return TABLE_EXPRESSION; }
    std::string type_str(){ return "TABLE_EXPRESSION"; }
    virtual NTableExpr* clone() const { return new NTableExpr(*this); }
    NExpression* evaluate(CodeExecutionContext& context) { return this; };
    void add_field(NExpression* valExpr);
    void add_field(NExpression* keyExpr, NExpression* valExpr, bool auto_incremented);
    void remove_field(NExpression* keyExpr);
    void sort_fields();
};

/*
    Multiple asignacion (y simple)

    A, B, C, D = "a", "b", "oops I took both c and d"

    idList = ["A", "B", "C", "D"]
    expressionList = ["a", "b", "oops I took both C and D"]

    Creo que idList.length() >= expressionList.length()
*/
class NMultiAssignment : public NStatement {
public:
    ExpressionList idList;
    ExpressionList expressionList;
    int isLocal;
    NMultiAssignment(ExpressionList idList, ExpressionList expressionList, int isLocal) :
        idList(idList), expressionList(expressionList), isLocal(isLocal) { }
    NodeType type(){ return MULTI_ASSIGNMENT; }
    std::string type_str(){ return "MULTI_ASSIGNMENT"; }
    NExpression* runCode(CodeExecutionContext& context);
};

class NLastStatement : public NStatement {
public:
    ExpressionList returnList;
    int isBreak;
    int fake; // Esta variable se usa para cuando se tiene un bloque que en realidad no tiene lastStatment.

    NLastStatement(int isBreak, ExpressionList& returnList):
        isBreak(isBreak), returnList(returnList), fake(0) { }
    NLastStatement(int isBreak, ExpressionList& returnList, int fake):
        isBreak(isBreak), returnList(returnList), fake(fake) { }
    NodeType type(){ return LAST_STATEMENT; }
    std::string type_str(){ return "LAST_STATEMENT"; }
    NExpression* runCode(CodeExecutionContext& context);
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

    NExpression* runCode(CodeExecutionContext& context);
    NodeType type(){ return BLOCK; }
    std::string type_str(){ return "BLOCK"; }
    virtual NBlock* clone() const { return new NBlock(*this); }
    NExpression* evaluate(CodeExecutionContext& context);
};

/*
    if algo then
        block
    elseif algo2 then
        block
    else
        block
    end
*/
class NIfCond : public NStatement {
public:
    NExpression& expression;
    NBlock& block;
    NIfCond(NExpression& expression, NBlock& block) :
        expression(expression), block(block) { }
    NodeType type(){ return IF_CONDITION; }
    std::string type_str(){ return "IF_CONDITION"; }
    NExpression* runCode(CodeExecutionContext& context);
};

class NIf : public NStatement {
public:
    ConditionList conditions;
    NIf(ConditionList& conditions) :
        conditions(conditions) { }
    NodeType type(){ return IF; }
    std::string type_str(){ return "IF"; }
    NExpression* runCode(CodeExecutionContext& context);
};

class NExpressionStatement : public NStatement {
public:
    NExpression& expression;
    NExpressionStatement(NExpression& expression) :
        expression(expression) { }
    NodeType type(){ return EXPRESSION_STATMENT; }
    std::string type_str(){ return "EXPRESSION_STATMENT"; }

    NExpression* runCode(CodeExecutionContext& context);
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
    std::string type_str(){ return "VARIABLE_DECLARATION"; }
    NExpression* runCode(CodeExecutionContext& context);
};

/*
    forloop usando in

    for key, value in pairs(t) do print(key, value) end

    nameList = ["key", "value"]
    expressionList = ["pairs(t)"]
    body = '''print(key, value)'''
*/
class NForLoopIn : public NStatement {
public:
    ExpressionList nameList;
    ExpressionList expressionList;
    NBlock& block;
    NForLoopIn(ExpressionList nameList, ExpressionList expressionList, NBlock& block) :
        nameList(nameList), expressionList(expressionList), block(block) { }
    NodeType type(){ return FOR_LOOP_IN; }
    std::string type_str(){ return "FOR_LOOP_IN"; }
    NExpression* runCode(CodeExecutionContext& context);
}; 

/*
    forloop con asignacion y limite e incremento definidos.

    for the_number = 1, MAXIMUM, STEP do print(the_number) end

    id = "the_number"
    expressionList = ["1", "MAXIMUM", "STEP"]
    body = ```print(the_number)```

    expressionList es de largo 2 o 3.
*/
class NForLoopAssign : public NStatement {
public:
    NIdentifier& id;
    ExpressionList expressionList;
    NBlock& block;
    NForLoopAssign(NIdentifier& id, ExpressionList expressionList, NBlock& block) :
        id(id), expressionList(expressionList), block(block) { }
    NodeType type(){ return FOR_LOOP_ASSIGN; }
    std::string type_str(){ return "FOR_LOOP_ASSIGN"; }
    NExpression* runCode(CodeExecutionContext& context);
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
    ExpressionList arguments;
    NBlock& block;
    NAnonFunctionDeclaration(ExpressionList& arguments, NBlock& block) :
        arguments(arguments), block(block) { }
    NodeType type(){ return ANON_FUNCTION_DECLARATION; }
    std::string type_str(){ return "ANON_FUNCTION_DECLARATION"; }
    virtual NAnonFunctionDeclaration* clone() const { return new NAnonFunctionDeclaration(*this); }
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
    ExpressionList arguments;
    NBlock& block;
    NFunctionDeclaration(NIdentifier& id, ExpressionList& arguments, NBlock& block) :
        id(id), arguments(arguments), block(block) { }
    NodeType type(){ return FUNCTION_DECLARATION; }
    std::string type_str(){ return "FUNCTION_DECLARATION"; }
    NExpression* runCode(CodeExecutionContext& context);
};

//Aux
double expressionToDouble(NExpression* expr);
int expressionToBoolean(NExpression* expr);
std::string expressionToString(NExpression* expr);
bool compareTableFields(NTableFieldExpression* a, NTableFieldExpression* b);

#endif NODE_H
