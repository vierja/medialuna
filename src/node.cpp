#include "node.h"
#include "codeexec.h"
#include "parser.hpp"
#include <math.h>
#include <sstream>
#include <iostream>

using namespace std;

void NBlock::runCode(CodeExecutionContext& context) {
    DEBUG_PRINT((RED"NBlock::runCode\n"RESET));
    DEBUG_PRINT((GREEN" - Size de statments: %i\n"RESET, (int)statements.size()));
    StatementList::const_iterator it;

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
        cout << "ERROR: Unexpected expression.\n";
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
        DEBUG_PRINT((YELLOW"Se evalua operator OR\n"RESET));
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
            if (boolean->trueVal == 0){
                return &lhs;
            }
        }

        // En el resto de los casos se devuelve el segundo.
        return rhs.evaluate(context);
    } else if (op == TK_OP_EQUALS || op == TK_OP_DIFF) {
        DEBUG_PRINT((YELLOW"Se evalua == o ~==\n"RESET));
        NodeType ltype = lhs.type();
        NodeType rtype = rhs.type();
        NExpression* lEvalExpression;
        NExpression* rEvalExpression;

        if (ltype == IDENTIFIER || ltype == FUNCTION_CALL || ltype == BINARY_OPERATOR || ltype == UNARY_OPERATOR || ltype == BLOCK || ltype == ANON_FUNCTION_DECLARATION){
            DEBUG_PRINT((YELLOW" - lhs es de tipo %s. Se evalua.\n"RESET, lhs.type_str().c_str()));
            lEvalExpression = lhs.evaluate(context);
            ltype = lEvalExpression->type();
            DEBUG_PRINT((YELLOW" - ahora lhs es de tipo %s.\n"RESET, lEvalExpression->type_str().c_str()));
        } else {
            DEBUG_PRINT((YELLOW" - lhs no es de tipo dinamico (%s). No se evalua.\n"RESET, lhs.type_str().c_str()));
            lEvalExpression = &lhs;
        }
        if (rtype == IDENTIFIER || rtype == FUNCTION_CALL || rtype == BINARY_OPERATOR || rtype == UNARY_OPERATOR || rtype == BLOCK || rtype == ANON_FUNCTION_DECLARATION){
            DEBUG_PRINT((YELLOW" - rhs es de tipo %s. Se evalua.\n"RESET, rhs.type_str().c_str()));
            rEvalExpression = rhs.evaluate(context);
            rtype = rEvalExpression->type();
            DEBUG_PRINT((YELLOW" - ahora rhs es de tipo %s.\n"RESET, rEvalExpression->type_str().c_str()));
        } else {
            DEBUG_PRINT((YELLOW" - rhs no es de tipo dinamico (%s). No se evalua.\n"RESET, rhs.type_str().c_str()));
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
                    trueVal = 1;
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
                trueVal = 1;
            else
                trueVal = 0;
            NBoolean* booleanResult = new NBoolean(trueVal);
            return booleanResult;
        }

        if (ltype == BOOLEAN){
            int intResult = 0;
            NBoolean* lBoolean = dynamic_cast<NBoolean*>(lEvalExpression);
            NBoolean* rBoolean = dynamic_cast<NBoolean*>(rEvalExpression);
            if ((lBoolean->trueVal == rBoolean->trueVal && op == TK_OP_EQUALS) || (lBoolean->trueVal != rBoolean->trueVal && op == TK_OP_DIFF)){
                DEBUG_PRINT((RED"   - Los dos son booleanos iguales: True.\n"RESET));
                intResult = 1;
            } else {
                DEBUG_PRINT((RED"   - Los dos son booleanos pero distintos: False.\n"RESET));
            }
            NBoolean* booleanResult = new NBoolean(intResult);
            return booleanResult;
        }

        if (ltype == STRING){
            int intResult = 0;
            NString* lString = dynamic_cast<NString*>(lEvalExpression);
            NString* rString = dynamic_cast<NString*>(rEvalExpression);
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
                NInteger* lInteger = dynamic_cast<NInteger*>(lEvalExpression);
                lValue = lInteger->value;    
            } else {
                NDouble* lDouble = dynamic_cast<NDouble*>(lEvalExpression);
                lValue = lDouble->value;    
            }

            if (rtype == INTEGER){
                NInteger* rInteger = dynamic_cast<NInteger*>(rEvalExpression);
                rValue = rInteger->value;    
            } else {
                NDouble* rDouble = dynamic_cast<NDouble*>(rEvalExpression);
                rValue = rDouble->value;    
            }
            
            if ((lValue == rValue && op == TK_OP_EQUALS) || (lValue != rValue && op == TK_OP_DIFF)){
                intResult = 1;
            }
            NBoolean* booleanResult = new NBoolean(intResult);
            return booleanResult;
        }
    } else if (op == TK_OP_MIN_EQUALS || op == TK_OP_GRT_EQUALS) {
        /*
            primer caso:
            a =< b
            segundo caso
            a => b

            pasamos el segundo caso al primer caso haciendo:

            b =< a
        */

        DEBUG_PRINT((YELLOW"Se evalua operator <= o =>\n"RESET));
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

    } else if (op == TK_OP_MIN || op == TK_OP_GRT) {
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
    } else if (op == TK_OP_DOTDOT){
        /*
        `..` es la operacion de concatenacion.
        Se puede realizar solo a STRING, o NUMBER
        */
        DEBUG_PRINT((YELLOW"Se evalua '..'\n"RESET));
        NodeType ltype = lhs.type();
        NodeType rtype = rhs.type();
        NExpression* lEvalExpression;
        NExpression* rEvalExpression;

        if (ltype == IDENTIFIER || ltype == FUNCTION_CALL || ltype == BINARY_OPERATOR || ltype == UNARY_OPERATOR || ltype == BLOCK || ltype == ANON_FUNCTION_DECLARATION){
            DEBUG_PRINT((YELLOW" - lhs es de tipo %s. Se evalua.\n"RESET, lhs.type_str().c_str()));
            lEvalExpression = lhs.evaluate(context);
            ltype = lEvalExpression->type();
            DEBUG_PRINT((YELLOW" - ahora lhs es de tipo %s.\n"RESET, lEvalExpression->type_str().c_str()));
        } else {
            DEBUG_PRINT((YELLOW" - lhs no es de tipo dinamico (%s). No se evalua.\n"RESET, lhs.type_str().c_str()));
            lEvalExpression = &lhs;
        }
        if (rtype == IDENTIFIER || rtype == FUNCTION_CALL || rtype == BINARY_OPERATOR || rtype == UNARY_OPERATOR || rtype == BLOCK || rtype == ANON_FUNCTION_DECLARATION){
            DEBUG_PRINT((YELLOW" - rhs es de tipo %s. Se evalua.\n"RESET, rhs.type_str().c_str()));
            rEvalExpression = rhs.evaluate(context);
            rtype = rEvalExpression->type();
            DEBUG_PRINT((YELLOW" - ahora rhs es de tipo %s.\n"RESET, rEvalExpression->type_str().c_str()));
        } else {
            DEBUG_PRINT((YELLOW" - rhs no es de tipo dinamico (%s). No se evalua.\n"RESET, rhs.type_str().c_str()));
            rEvalExpression = &rhs;
        }

        if ((ltype != STRING && ltype != INTEGER && ltype != DOUBLE) || (rtype != STRING && rtype != INTEGER && rtype != DOUBLE)){
            cout << "ERROR: Invalid value for `..` (concat).\n";
            exit(0);
        }

        // Paso los valores a string y luego los concateno.
        string lstring;
        string rstring;
        // Para hacer las conversiones.
        stringstream rstrstream, lstrstream;

        if (ltype == STRING) {
            lstring = (dynamic_cast<NString*>(lEvalExpression))->value;
        } else if (ltype == INTEGER) {
            lstrstream << (dynamic_cast<NInteger*>(lEvalExpression))->value;
            lstrstream >> lstring;
        } else if (ltype == DOUBLE) {
            lstrstream << (dynamic_cast<NDouble*>(lEvalExpression))->value;
            lstrstream >> lstring;
        }
        DEBUG_PRINT((YELLOW" - lhs = '%s'.\n"RESET, lstring.c_str()));

        if (rtype == STRING) {
            rstring = (dynamic_cast<NString*>(rEvalExpression))->value;
        } else if (rtype == INTEGER) {
            rstrstream << (dynamic_cast<NInteger*>(rEvalExpression))->value;
            rstrstream >> rstring;
        } else if (rtype == DOUBLE) {
            rstrstream << (dynamic_cast<NDouble*>(rEvalExpression))->value;
            rstrstream >> rstring;
        }
        DEBUG_PRINT((YELLOW" - rhs = '%s'.\n"RESET, rstring.c_str()));

        string result = lstring + rstring;
        NString* resString = new NString(result);
        return resString;

    } else if (op == TK_OP_PLUS || op == TK_OP_MINUS || op == TK_OP_TIMES || op == TK_OP_DIVIDED || op == TK_OP_MOD || op == TK_OP_EXP) {
        /*
            Las operaciones numericas. 
            Si alguna de las expressiones es string entonces se intenta pasar a numero.
            Si falla la conversion entonces falla el operador.
        */
        DEBUG_PRINT((YELLOW"Se evalua operator de numeros\n"RESET));
        NodeType ltype = lhs.type();
        NodeType rtype = rhs.type();
        NExpression* lEvalExpression;
        NExpression* rEvalExpression;

        if (ltype == IDENTIFIER || ltype == FUNCTION_CALL || ltype == BINARY_OPERATOR || ltype == UNARY_OPERATOR || ltype == BLOCK || ltype == ANON_FUNCTION_DECLARATION){
            DEBUG_PRINT((YELLOW" - lhs es de tipo %s. Se evalua.\n"RESET, lhs.type_str().c_str()));
            lEvalExpression = lhs.evaluate(context);
            ltype = lEvalExpression->type();
            DEBUG_PRINT((YELLOW" - ahora lhs es de tipo %s.\n"RESET, lEvalExpression->type_str().c_str()));
        } else {
            DEBUG_PRINT((YELLOW" - lhs no es de tipo dinamico (%s). No se evalua.\n"RESET, lhs.type_str().c_str()));
            lEvalExpression = &lhs;
        }
        if (rtype == IDENTIFIER || rtype == FUNCTION_CALL || rtype == BINARY_OPERATOR || rtype == UNARY_OPERATOR || rtype == BLOCK || rtype == ANON_FUNCTION_DECLARATION){
            DEBUG_PRINT((YELLOW" - rhs es de tipo %s. Se evalua.\n"RESET, rhs.type_str().c_str()));
            rEvalExpression = rhs.evaluate(context);
            rtype = rEvalExpression->type();
            DEBUG_PRINT((YELLOW" - ahora rhs es de tipo %s.\n"RESET, rEvalExpression->type_str().c_str()));
        } else {
            DEBUG_PRINT((YELLOW" - rhs no es de tipo dinamico (%s). No se evalua.\n"RESET, rhs.type_str().c_str()));
            rEvalExpression = &rhs;
        }

        if ((ltype != STRING && ltype != INTEGER && ltype != DOUBLE) || (rtype != STRING && rtype != INTEGER && rtype != DOUBLE)){
            cout << "ERROR: Invalid value expression.\n";
            exit(0);
        }

        // Si los dos valores son Integer entonces el resultado es INT.
        if (ltype == INTEGER && rtype == INTEGER) {
            DEBUG_PRINT((YELLOW" - Ambos son integer. Obtengo los valores.\n"RESET));

            long lIntVal = (dynamic_cast<NInteger*>(lEvalExpression))->value;
            long rIntVal = (dynamic_cast<NInteger*>(rEvalExpression))->value;

            DEBUG_PRINT((YELLOW" - Ambos son integer: lIntVal: %ld, rIntVal: %ld\n"RESET, lIntVal, rIntVal));

            long intResult;
            if (op == TK_OP_PLUS) {
                intResult = lIntVal + rIntVal;
            } else if (op == TK_OP_MINUS) {
                intResult = lIntVal - rIntVal;
            } else if (op == TK_OP_TIMES) {
                intResult = lIntVal * rIntVal;
            } else if (op == TK_OP_MOD) {
                intResult = lIntVal % rIntVal;
            } else if (op == TK_OP_EXP){
                intResult = pow(lIntVal, rIntVal);
            } else if (op == TK_OP_DIVIDED) {
                double dResult = (double)lIntVal / (double)rIntVal;
                NDouble* doubleResult = new NDouble(dResult);
                return doubleResult;
            }

            NInteger* integerResult = new NInteger(intResult);
            return integerResult;
        }

        double lDoubleValue;
        // Si es String lo tratamos de pasar a double, o si fallamos a integer.
        if (ltype == STRING) {
            // Primero tratamos a double.
            istringstream ss((dynamic_cast<NString*>(lEvalExpression))->value);
            if (!(ss >> lDoubleValue)){
                // No es double.
                cout << "ERROR: Invalid string value for left side expression.\n";
                exit(0);
            }
        } else if (ltype == DOUBLE) {
            // Si left es double entonces right lo va a ser tambien (por que int int ya termino.)
            lDoubleValue = (dynamic_cast<NDouble*>(lEvalExpression))->value;
            DEBUG_PRINT((GREEN" - Se obtiene el res del double: %f\n"RESET, lDoubleValue));
        } else if (ltype == INTEGER) {
            lDoubleValue = (double)(dynamic_cast<NInteger*>(lEvalExpression))->value;
        }

        double rDoubleValue;
        // Si es String lo tratamos de pasar a double, o si fallamos a integer.
        if (rtype == STRING) {
            // Primero tratamos a double.
            istringstream ss((dynamic_cast<NString*>(rEvalExpression))->value);
            if (!(ss >> rDoubleValue)){
                // No es double.
                cout << "ERROR: Invalid string value for right side expression.\n";
                exit(0);
            }
        } else if (rtype == DOUBLE) {
            // Si left es double entonces right lo va a ser tambien (por que int int ya termino.)
            rDoubleValue = (dynamic_cast<NDouble*>(rEvalExpression))->value;
        } else if (rtype == INTEGER) {
            rDoubleValue = (double)(dynamic_cast<NInteger*>(rEvalExpression))->value;
        }

        double dResult;
        if (op == TK_OP_PLUS) {
            dResult = lDoubleValue + rDoubleValue;
        } else if (op == TK_OP_MINUS) {
            dResult = lDoubleValue - rDoubleValue;
        } else if (op == TK_OP_TIMES) {
            dResult = lDoubleValue * rDoubleValue;
        } else if (op == TK_OP_DIVIDED) {
            dResult = lDoubleValue / rDoubleValue;
        } else if (op == TK_OP_MOD) {
            dResult = fmod(lDoubleValue,rDoubleValue);
        } else if (op == TK_OP_EXP){
            dResult = pow(lDoubleValue, rDoubleValue);
        }
        NDouble* doubleResult = new NDouble(dResult);
        return doubleResult;
    } else {
        cout << "ERROR: Unknown operator.\n";
        exit(0);
    }
}

NExpression* NUnaryOperator::evaluate(CodeExecutionContext& context){
    if (op == TK_OP_MINUS) {
        /*
            Negativo de la expression. Solo para numeros o string pasandolos a numeros.
        */
        // Evaluo la expresion si es necesario.
        DEBUG_PRINT((YELLOW"Se evalua '-' unario\n"RESET));
        NodeType rtype = rhs.type();
        NExpression* rEvalExpression;

        if (rtype == IDENTIFIER || rtype == FUNCTION_CALL || rtype == BINARY_OPERATOR || rtype == UNARY_OPERATOR || rtype == BLOCK || rtype == ANON_FUNCTION_DECLARATION){
            DEBUG_PRINT((YELLOW" - rhs es de tipo %s. Se evalua.\n"RESET, rhs.type_str().c_str()));
            rEvalExpression = rhs.evaluate(context);
            rtype = rEvalExpression->type();
            DEBUG_PRINT((YELLOW" - ahora rhs es de tipo %s.\n"RESET, rEvalExpression->type_str().c_str()));
        } else {
            DEBUG_PRINT((YELLOW" - rhs no es de tipo dinamico (%s). No se evalua.\n"RESET, rhs.type_str().c_str()));
            rEvalExpression = &rhs;
        }

        if (rtype != STRING && rtype != INTEGER && rtype != DOUBLE){
            cout << "ERROR: Invalid value expression for unary '-'.\n";
            exit(0);
        }

        if (rtype == INTEGER) {
            long rIntVal = (dynamic_cast<NInteger*>(rEvalExpression))->value;
            rIntVal = -rIntVal;
            NInteger* resNumber = new NInteger(rIntVal);
            return resNumber;
        }

        double rDoubleValue;
        // Si es String lo tratamos de pasar a double, o si fallamos a integer.
        if (rtype == STRING) {
            // Primero tratamos a double.
            istringstream ss((dynamic_cast<NString*>(rEvalExpression))->value);
            if (!(ss >> rDoubleValue)){
                // No es double.
                cout << "ERROR: Invalid string value for unary '-'.\n";
                exit(0);
            }
        } else if (rtype == DOUBLE) {
            // Si left es double entonces right lo va a ser tambien (por que int int ya termino.)
            rDoubleValue = (dynamic_cast<NDouble*>(rEvalExpression))->value;
            DEBUG_PRINT((GREEN" - Se obtiene el res del double: %f\n"RESET, rDoubleValue));
        }

        NDouble* resNumber = new NDouble(rDoubleValue);
        return resNumber;



    } else if (op == TK_KW_NOT) {
        /*
            Negativo de la evaluacion.
            El operador not devuelve solo true o false
            En los unicos casos que devuelve True es en los casos
            que la expression es False o Nil.
            En el resto devuelve valse.
        */

        // Evaluo la expresion si es necesario.
        DEBUG_PRINT((YELLOW"Se evalua 'not'\n"RESET));
        NodeType rtype = rhs.type();
        NExpression* rEvalExpression;

        if (rtype == IDENTIFIER || rtype == FUNCTION_CALL || rtype == BINARY_OPERATOR || rtype == UNARY_OPERATOR || rtype == BLOCK || rtype == ANON_FUNCTION_DECLARATION){
            DEBUG_PRINT((YELLOW" - rhs es de tipo %s. Se evalua.\n"RESET, rhs.type_str().c_str()));
            rEvalExpression = rhs.evaluate(context);
            rtype = rEvalExpression->type();
            DEBUG_PRINT((YELLOW" - ahora rhs es de tipo %s.\n"RESET, rEvalExpression->type_str().c_str()));
        } else {
            DEBUG_PRINT((YELLOW" - rhs no es de tipo dinamico (%s). No se evalua.\n"RESET, rhs.type_str().c_str()));
            rEvalExpression = &rhs;
        }

        if (rtype == BOOLEAN) {
            NBoolean* boolean = dynamic_cast<NBoolean*>(rEvalExpression);
            // Si es False entonces True.
            if (boolean->trueVal == 0){
                NBoolean* resBoolean = new NBoolean(1);
                return resBoolean;
            }
        }

        if (rtype == NIL) {
            NBoolean* resBoolean = new NBoolean(1);
            return resBoolean;
        }

        // Else devuelvo false.
        NBoolean* resBoolean = new NBoolean(0);
        return resBoolean;
    }
}

NExpression* NBlock::evaluate(CodeExecutionContext& context){
    
}

NExpression* NAnonFunctionDeclaration::evaluate(CodeExecutionContext& context){
    
}