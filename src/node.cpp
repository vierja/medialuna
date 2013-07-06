#include "node.h"
#include "codeexec.h"
#include "parser.hpp"

using namespace std;

void NBlock::runCode(CodeExecutionContext& context) {
    cout << "NBlock::runCode" << endl;
    StatementList::const_iterator it;
    cout << "Size de statments: " <<  statements.size() << endl;
    for (it = statements.begin(); it != statements.end(); it++){
        cout << "Running code for ";
        cout << typeid(**it).name() << endl;
        (**it).runCode(context);
        cout << "Corrido\n";
    }
    cout << "Se termina de agregar los statemtents";
    context.printFunctionsAndVariables();
    
    return lastStatement.runCode(context);
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
    context.addFunction(funcName, this);
    cout << "agregada al contexto" << endl;
}

void NMultiAssignment::runCode(CodeExecutionContext& context) {
    /*
        Cuando se tiene multi assignment hay varias cosas para tener
        en cuenta.
        Si es local entonces hay que ver como tener prioridad ante el resto.

        IdentifierList idList;
        ExpressionList expresionList;
        int isLocal;
    */
    IdentifierList::const_iterator id_it;
    ExpressionList::const_iterator exp_it = expresionList.begin();
    for (id_it = idList.begin(); id_it != idList.end(); id_it++){
        string varName = (**id_it).name;
        NExpression* varExpression;
        if (exp_it != expresionList.end()){
            // Si es una funcion entonces tengo que evaluarla.
            // y si es una variable tengo que obtener su valor.
            varExpression = (*exp_it)->evaluate(context);
        } else {
            varExpression = new NNil();
        }
        cout << "Se declara la variable " << varName << endl;
        context.addVariable(varName, varExpression, isLocal);

        if (exp_it != expresionList.end()){
            exp_it++;
        }
    }
}

void NExpressionStatement::runCode(CodeExecutionContext& context) {
    /*
        Cuando se tiene un expression statment lo unico que importa
        es si es es NFunctionCall, en ese caso se llama a la funcion.
    */
    /* TODO: Esto deberia de arreglarse en el parser. */
    if (expression.type() != FUNCTION_CALL){
        cout << "Unexpected expression." << endl;
        exit(1);
    }
    cout << "NExpressionStatement::runCode" << endl;
    NFunctionCall* functionCall = dynamic_cast<NFunctionCall*>(&expression);
    string funcName = functionCall->id.name;

    if (funcName.compare("print") == 0){
        context.print(functionCall->arguments);
    }


}


void NLastStatement::runCode(CodeExecutionContext& context) {

}

void NForLoopIn::runCode(CodeExecutionContext& context) {

}

void NForLoopAssign::runCode(CodeExecutionContext& context) {

}

/*
void NMultiVariableDeclaration::runCode(CodeExecutionContext& context) {

}
*/

NExpression* NIdentifier::evaluate(CodeExecutionContext& context){
    //Busco el valor de la variable en el contexto.
    cout << "NIdentifier::evaluate\n";
    //Tengo que obtener el valor del identificador.
    return context.getVariable(name)->evaluate(context);
}

NExpression* NFunctionCall::evaluate(CodeExecutionContext& context){
    cout << "NFunctionCall::evaluate\n";
    con
}

NExpression* NBinaryOperator::evaluate(CodeExecutionContext& context){
    /*
        Tipos posibles de expression:
            NIL
            BOOLEAN
            INTEGER
            DOUBLE
            STRING
            IDENTIFIER
            FUNCTION_CALL
            BINARY_OPERATOR
            UNARY_OPERATOR
            BLOCK
            ANON_FUNCTION_DECLARATION
    */
    if (op == TK_KW_OR) {
        NodeType ltype = lhs.type();
        NExpression* lEvalExpression;

         // Si es de un tipo que hay que evaluar, se evalua primero y luego flujo normal.
        if (ltype == IDENTIFIER || ltype == FUNCTION_CALL || ltype == BINARY_OPERATOR || ltype == UNARY_OPERATOR || ltype == BLOCK || ltype == ANON_FUNCTION_DECLARATION){
            lEvalExpression = lhs.evaluate(context);
            ltype = lEvalExpression->type();
        } else {
            lEvalExpression = &lhs;
        }

        // Si el primer valor es Int, Double o String entonces 
        // automaticamente devuelvo ese valor.
        if (ltype == INTEGER || ltype == DOUBLE || ltype == STRING){
            return &lhs;
        }

        // Esta variable sirve para saber si se tiene que evaluar
        // al segundo operador.
        if (ltype == BOOLEAN){
            NBoolean* boolean = dynamic_cast<NBoolean*>(&lhs);
            // Si se evalua el left true entonces devolvemos true.
            if (boolean->trueVal){
                return &lhs;
            } else {
                // Si no devolvemos la evaluacion del segundo.
                return rhs.evaluate(context);
            }
        }

        //Si el primer valor es nil entonces devolvemos la evaluacion
        // del segundo operador.
        if (ltype == NIL){
            return rhs.evaluate(context);
        }
    } else if (op == TK_KW_AND) {
        NodeType ltype = lhs.type();
        NExpression* lEvalExpression;

         // Si es de un tipo que hay que evaluar, se evalua primero y luego flujo normal.
        if (ltype == IDENTIFIER || ltype == FUNCTION_CALL || ltype == BINARY_OPERATOR || ltype == UNARY_OPERATOR || ltype == BLOCK || ltype == ANON_FUNCTION_DECLARATION){
            lEvalExpression = lhs.evaluate(context);
            ltype = lEvalExpression->type();
        } else {
            lEvalExpression = &lhs;
        }

        // Si el primero es Nil, se devuelve nil
        if (ltype == NIL){
            return &lhs;
        }

        // Si el primero es booleano False, se devuelve False.
        if (ltype == BOOLEAN){
            NBoolean* boolean = dynamic_cast<NBoolean*>(&lhs);
            if (!boolean->trueVal){
                return &lhs;
            }
        }

        // En el resto de los casos se devuelve el segundo.
        return rhs.evaluate(context);
    } else if (op == TK_OP_EQUALS || op == TK_OP_DIFF) {
        NodeType ltype = lhs.type();
        NodeType rtype = lhs.type();
        NExpression* lEvalExpression;
        NExpression* rEvalExpression;

        if (ltype == IDENTIFIER || ltype == FUNCTION_CALL || ltype == BINARY_OPERATOR || ltype == UNARY_OPERATOR || ltype == BLOCK || ltype == ANON_FUNCTION_DECLARATION){
            lEvalExpression = lhs.evaluate(context);
            ltype = lEvalExpression->type();
        } else {
            lEvalExpression = &lhs;
        }
        if (rtype == IDENTIFIER || rtype == FUNCTION_CALL || rtype == BINARY_OPERATOR || rtype == UNARY_OPERATOR || rtype == BLOCK || rtype == ANON_FUNCTION_DECLARATION){
            rEvalExpression = rhs.evaluate(context);
            rtype = rEvalExpression->type();
        } else {
            rEvalExpression = &rhs;
        }

        // Si no son de tipo INTEGER o DOUBLE y son de distinto tipo entonces false.
        // (Hago dos ifs para verlo mejor)
        if (ltype != INTEGER && rtype != INTEGER && ltype != DOUBLE && rtype != DOUBLE){
            // Si el tipo de los nodos es distinto entonces se devuelve False.
            if (ltype != rtype){
                int trueVal;
                if (op == TK_OP_EQUALS)
                    trueVal = 0;
                else
                    trueVal = 1
                NBoolean* booleanResult = new NBoolean(trueVal);
                return booleanResult;
            }
        }

        /*
            TODO:
            En caso que se comparen funciones, no funcioncall.
            Por ejemplo
            function a() return 0 end
            function b() return 0 end
            (a == b) == false

            pero 
            (a() == b()) == true

            El primer caso hay que ver como hacerlo.
        */
        if (ltype == NIL){
            // nil == nil es true
            int trueVal;
            if (op == TK_OP_EQUALS)
                trueVal = 0;
            else
                trueVal = 1
            NBoolean* booleanResult = new NBoolean(trueVal);
            return booleanResult;
        }

        if (ltype == BOOLEAN){
            int intResult = 0;
            NBoolean* lBoolean = dynamic_cast<NBoolean*>(&lhs);
            NBoolean* rBoolean = dynamic_cast<NBoolean*>(&lhs);
            if ((lBoolean->trueVal == rBoolean->trueVal && op == TK_OP_EQUALS) || (lBoolean->trueVal != rBoolean->trueVal && op == TK_OP_DIFF)){
                intResult = 1;
            }
            NBoolean* booleanResult = new NBoolean(intResult);
            return booleanResult;
        }

        if (ltype == STRING){
            int intResult = 0;
            NString* lString = dynamic_cast<NString*>(&lhs);
            NString* rString = dynamic_cast<NString*>(&lhs);
            if ((lString->value.compare(rString->value) == 0 && op == TK_OP_EQUALS) || (lString->value.compare(rString->value) != 0 && op == TK_OP_DIFF)){
                intResult = 1;
            }
            NBoolean* booleanResult = new NBoolean(intResult);
            return booleanResult;
        }

        if (ltype == INTEGER || ltype == DOUBLE){
            int intResult = 0;
            double lValue;
            double rValue;
            if (ltype == INTEGER){
                NInteger* lInteger = dynamic_cast<NInteger*>(&lhs);
                lValue = lInteger->value;    
            } else {
                NDouble* lDouble = dynamic_cast<NDouble*>(&lhs);
                lValue = lDouble->value;    
            }

            if (rtype == INTEGER){
                NInteger* rInteger = dynamic_cast<NInteger*>(&lhs);
                rValue = rInteger->value;    
            } else {
                NDouble* rDouble = dynamic_cast<NDouble*>(&lhs);
                rValue = rDouble->value;    
            }
            
            if ((lValue == rValue && op == TK_OP_EQUALS) || (lValue != rValue && op == TK_OP_DIFF)){
                intResult = 1;
            }
            NBoolean* booleanResult = new NBoolean(intResult);
            return booleanResult;
        }
    } else if (op == TK_OP_MIN_EQUALS || op == TK_OP_GRT_EQUALS){
        /*
            primer caso:
            a =< b
            segundo caso
            a => b

            pasamos el segundo caso al primer caso haciendo:

            b =< a
        */

        NExpression* lefths;
        NExpression* righths;

        if (op == TK_OP_GRT_EQUALS){
            // Invierto los operadores de lugar.
            lefths = &rhs;
            righths = &lhs;
        } else {
            // caso normal TK_OP_MIN_EQUALS
            lefths = &lhs;
            righths = &rhs;
        }

        /* TODO: Implementar logica */

    } else if (op == TK_OP_MIN || op == TK_OP_GRT){
        /*
            Al igual que con TK_OP_MIN_EQUALS y TK_OP_GRT_EQUALS
            cuando tenemos TK_OP_GRT invertimos de lugar para 
            no repetir logica.
        */
        NExpression* lefths;
        NExpression* righths;

        if (op == TK_OP_GRT){
            // Invierto los operadores de lugar.
            lefths = &rhs;
            righths = &lhs;
        } else {
            // caso normal TK_OP_MIN
            lefths = &lhs;
            righths = &rhs;
        }

        /* TODO: Implementar logica */
    }
}

NExpression* NUnaryOperator::evaluate(CodeExecutionContext& context){
    
}

NExpression* NBlock::evaluate(CodeExecutionContext& context){
    
}

NExpression* NAnonFunctionDeclaration::evaluate(CodeExecutionContext& context){
    
}