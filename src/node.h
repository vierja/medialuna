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

class NStatement : public Node {

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

class NIdentifier : public NExpression {
public:
    std::string name;
    NIdentifier(const std::string& name) : name(name) { }
};

class NMethodCall : public NExpression {
public:
    const NIdentifier& id;
    ExpressionList arguments;
    NMethodCall(const NIdentifier& id, ExpressionList& arguments) :
        id(id), arguments(arguments) { }
    NMethodCall(const NIdentifier& id) : id(id) { }
};

class NBinaryOperator : public NExpression {
public:
    int op;
    NExpression& lhs;
    NExpression& rhs;
    NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
        lhs(lhs), rhs(rhs), op(op) { }
};

class NAssignment : public NExpression {
public:
    NIdentifier& lhs;
    NExpression& rhs;
    NAssignment(NIdentifier& lhs, NExpression& rhs) : 
        lhs(lhs), rhs(rhs) { }
};

class NBlock : public NExpression {
public:
    StatementList localStatements;
    StatementList statements;
    NBlock(StatementList& localStatements, StatementList& statements) {
        localStatements(localStatements);
        statements(statements)
    }
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
    ExpressionList assignExpresionList;
    NMultiVariableDeclaration(IdentifierList idList, ExpressionList assignExpresionList):
        idList(idList), assignExpresionList(assignExpresionList) { }

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

class NFunctionDeclaration : public NStatement {
public:
    const NIdentifier& id;
    VariableList arguments;
    NBlock& block;
    NFunctionDeclaration(const NIdentifier& id, const VariableList& arguments, NBlock& block) :
        id(id), arguments(arguments), block(block) { }
};

#endif NODE_H
