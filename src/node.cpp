#include "node.h"
#include "codeexec.h"
#include "parser.hpp"
#include <math.h>
#include <sstream>
#include <iostream>
#include <algorithm>

using namespace std;


NExpression* NBlock::runCode(CodeExecutionContext& context) {
    DEBUG_PRINT((MAGENTA"NBlock::runCode\n"RESET));
    DEBUG_PRINT((MAGENTA" - Size de statments: %i\n"RESET, (int)statements.size()));
    StatementList::const_iterator it;
    int count = 0;
    NExpression* statmentRes;

    for (it = statements.begin(); it != statements.end(); it++){
        DEBUG_PRINT((MAGENTA"   ** Se corre el statement numero %d, tipo: %s\n"RESET, count, typeid(**it).name()));
        statmentRes = (**it).runCode(context);
        if (statmentRes != 0){
            DEBUG_PRINT((MAGENTA"   ** Statement numero %d devuelve valor no null, tipo: %s.\n"RESET, count++, statmentRes->type_str().c_str()));
            if (statmentRes->type() != NIL){
                return statmentRes;
            }
        }
        DEBUG_PRINT((MAGENTA"   ** Fin del statement numero %d\n"RESET, count++));
    }
    DEBUG_PRINT((BLUE"Se imprimen variables y funciones:\n"RESET));
    context.printFunctionsAndVariables();
    DEBUG_PRINT((BLUE"Se terminan de imprimir variables\n"RESET));

    DEBUG_PRINT((BLUE"Se terminan los statements y se ejecuta lastStatement de tipo: %s\n"RESET, lastStatement.type_str().c_str()));
    NExpression* resultExpr = lastStatement.runCode(context);
    DEBUG_PRINT((BLUE"Se evalua y el res es de tipo: %s\n"RESET, resultExpr->type_str().c_str()));
    return resultExpr;
}

NExpression* NFunctionDeclaration::runCode(CodeExecutionContext& context) {
    /*
        Cuando se declara una funcion no se corre, pero se debe guardar 
        en el contexto.
        ¿Como se guarda?

        NIdentifier& id;
        IdentifierList arguments;
        NBlock& block;
    */
    string funcName = id.name;
    context.addFunction(funcName, this);
    
    // No devuelve nada.
    return new NNil();

}

NExpression* NMultiAssignment::runCode(CodeExecutionContext& context) {
    /*
        Cuando se tiene multi assignment hay varias cosas para tener
        en cuenta.
        Si es local entonces hay que ver como tener prioridad ante el resto.

        IdentifierList idList;
        ExpressionList expressionList;
        int isLocal;
    */

    IdentifierList::const_iterator id_it;
    ExpressionList::const_iterator exp_it = expressionList.begin();
    for (id_it = idList.begin(); id_it != idList.end(); id_it++){
        string varName = (**id_it).name;
        NExpression* varExpression;
        if (exp_it != expressionList.end()){
            DEBUG_PRINT((RED"Variable con expression de tipo %s. Se evalua.\n"RESET, (*exp_it)->type_str().c_str()));
            // Si es una funcion entonces tengo que evaluarla.
            // y si es una variable tengo que obtener su valor.
            varExpression = (*exp_it)->evaluate(context);
            DEBUG_PRINT((RED"La expression devuelve tipo: %s.\n"RESET, varExpression->type_str().c_str()));
        } else {
            DEBUG_PRINT((RED"Variable sin expression. Se setea como null."RESET));
            varExpression = new NNil();
        }
        DEBUG_PRINT(("Se declara la variable %s\n",varName.c_str()));
        context.addVariable(varName, varExpression, isLocal);
        DEBUG_PRINT((YELLOW"Variable %s declarada\n"RESET, varName.c_str()));

        if (exp_it != expressionList.end()){
            exp_it++;
        }
    }
    return new NNil();
}

NExpression* NIfCond::runCode(CodeExecutionContext& context) {
    DEBUG_PRINT((RED"NIfCond::runCode -> NO SE DEBERIA LLAMAR!!\n"RESET));
}

NExpression* NIf::runCode(CodeExecutionContext& context) {
    /*
        NIf tiene una lista de NIfCond en ConditionList conditions.
        pseudo:
        for NIfCond in conditions:
            if NIfCond->expression.evaluate(context):
                res = NIfCond->block.runCode(context)
                if res != null:
                    return res
                break
    */
    int cond_count = 0;
    DEBUG_PRINT((RED"NIf::runCode\n"RESET));
    ConditionList::const_iterator cond_it;
    NExpression* returnExpr = new NNil();

    for (cond_it = conditions.begin(); cond_it != conditions.end(); cond_it++) {
        // Calculo el valor booleano de la expression del if en la condicion.
        int boolValue = expressionToBoolean((**cond_it).expression.evaluate(context));
        if (boolValue == 1) {
            // Entramos al if. Corremos el codigo, y termina.
            CodeExecutionBlock* ifBlock = new CodeExecutionBlock((**cond_it).block);
            context.push_block(ifBlock);
            DEBUG_PRINT((GREEN"Se ejecuta el bloque del if num: %d .\n"RESET, cond_count));
            returnExpr = (**cond_it).block.runCode(context);
            DEBUG_PRINT((GREEN"Se termina de ejecutar bloque del if num: %d.\n"RESET, cond_count));
            context.pop_block();
            break;
        }
        cond_count++;
    }

    return returnExpr;
}

NExpression* NExpressionStatement::runCode(CodeExecutionContext& context) {
    /*

        NExpression& expression;


        Cuando se tiene un expression statment lo unico que importa
        es si es es NFunctionCall, en ese caso se llama a la funcion.
    */

    if (expression.type() != FUNCTION_CALL){
        cout << "ERROR: Unexpected expression.\n";
        exit(1);
    }
    DEBUG_PRINT(("NExpressionStatement::runCode\n"));
    return expression.evaluate(context);
}

NExpression* NLastStatement::runCode(CodeExecutionContext& context) {
    /*
        NLastStatement
        Puede ser un break o un return.
        En caso de ser return tiene una ExpressionList (returnList)
    */
    DEBUG_PRINT((RED"NLastStatement::runCode.\n"RESET));
    if (fake == 1){
        DEBUG_PRINT((RED" - es Fake osea, Nul.\n"RESET));
        NNil* nilRes = new NNil();
        return nilRes;
    } else if (isBreak){
        DEBUG_PRINT((RED" - es break.\n"RESET));
        NBreak* breakRes = new NBreak();
        return breakRes;
    } else if (returnList.size() == 1) {
        DEBUG_PRINT((RED" - solo una expression de tipo %s. Se evalua.\n"RESET, returnList[0]->type_str().c_str()));
        NExpression* onlyExpr = returnList[0];
        return onlyExpr->evaluate(context);
    } else {
        DEBUG_PRINT((RED" - es una expression list.\n"RESET));
        // Sino, tengo que evaluar de ExpressionList.
        ExpressionList* evalExpList = new ExpressionList();
        ExpressionList::const_iterator it;

        for (it = returnList.begin(); it != returnList.end(); it++){
            evalExpList->push_back((**it).evaluate(context));
        }

        NExpressionList* evalNExpr = new NExpressionList(*evalExpList);
        return evalNExpr;
    }

}

NExpression* NForLoopIn::runCode(CodeExecutionContext& context) {
    /*
        No es obligatorio, se deja para despues.
    */
}

NExpression* NForLoopAssign::runCode(CodeExecutionContext& context) {
    /*
        NIdentifier& id;
        ExpressionList expressionList;
        NBlock& block;

        id es la variable que se declara.
        expressionList es de largo 2 o 3.
        El primer valor determina el valor de id inicial.
        El segundo valor es el limite.
        Y en caso de no parar, si es que existe, se le suma el valor a la variable.
    */
    if (expressionList.size() < 2 || expressionList.size() > 3){
        cout << "ERROR: Invalid parameters in for declaration\n";
        exit(0);
    }

    // Las expressiones se evaluan en el contexto actual.
    NExpression* firstExpr = expressionList[0]->evaluate(context);
    DEBUG_PRINT(("Se evalua primera expression\n"));
    NExpression* secondExpr = expressionList[1]->evaluate(context);
    DEBUG_PRINT(("Se evalua segunda expression\n"));
    NExpression* thirdExpr;
    int thirdExpressionDef = 0;
    if (expressionList.size() == 3) {
        thirdExpr = expressionList[2]->evaluate(context);
        thirdExpressionDef = 1;
    }

    // Si alguna de las expresiones no son numeros entonces es un for invalido.
    double firstVal;
    try{
        firstVal = expressionToDouble(firstExpr);
    } catch (int e) {
        cout << "ERROR: 'for' initial value must be a number.\n";
        exit(0);
    }
    DEBUG_PRINT((GREEN"ForLoop: firstVal = %f\n"RESET, firstVal));
    double secondVal;
    try{
        secondVal = expressionToDouble(secondExpr);
    } catch (int e) {
        cout << "ERROR: 'for' second value must be a number.\n";
        exit(0);
    }
    DEBUG_PRINT((GREEN"ForLoop: secondVal = %f\n"RESET, secondVal));

    double thirdVal = 1;
    if (thirdExpressionDef == 1){
        try{
            thirdVal = expressionToDouble(thirdExpr);
        } catch (int e) {
            cout << "ERROR: 'for' third value must be a number.\n";
            exit(0);
        }
        DEBUG_PRINT((GREEN"ForLoop: thirdVal = %f\n"RESET, thirdVal));
    }
    

    int blockReturned = 0;
    int blockBreaked = 0;
    int loopEndend = 0;
    double varVal = firstVal;
    int forCount = 0;
    NExpression* forReturn;

    while (1){

        if ((thirdVal > 0 && varVal > secondVal) || (thirdVal < 0 && varVal < secondVal)){
            loopEndend = 1;
            break;
        }
        CodeExecutionBlock* forBlock = new CodeExecutionBlock(block);
        context.push_block(forBlock);

        NExpression* firstExprDouble = new NDouble(varVal);
        context.addVariable(id.name, firstExprDouble, 1);

        DEBUG_PRINT((GREEN"Se ejecuta el bloque del for num: %d .\n"RESET, forCount++));
        NExpression* res = block.runCode(context);
        DEBUG_PRINT((GREEN"Se termina de ejecutar bloque del for.\n"RESET));

        firstExpr = context.getVariable(id.name);
        try{
            varVal = expressionToDouble(firstExpr);
        } catch (int e) {
            // Si se captura una excepcion entonces se considera como un break;
            blockBreaked = 1;
            break;
        }

        if (res->type() == BREAK) {
            blockBreaked = 1;
            break;
        } else if (res->type() != NIL) {
            // Si el restul es distinto de nil entonces retorna ALGO.
            forReturn = res;
            blockReturned = 1;
            break;
        } // el es NIL entonces sigue normal.

        context.pop_block();

        varVal = varVal + thirdVal;

    }



    if (blockBreaked == 1){
        // Si se hizo break, devuelve null.
        NNil* nilReturn = new NNil();
        return nilReturn;
    }

    if (blockReturned == 1){
        // Se guarda el return en forReturnList
        return forReturn;
    }

    // else
    return new NNil();
}

/*
NExpression* NMultiVariableDeclaration::runCode(CodeExecutionContext& context) {

}
*/

NExpression* NExpressionList::evaluate(CodeExecutionContext& context) {
    ExpressionList* evalExpList = new ExpressionList();
    ExpressionList::const_iterator it;

    for (it = exprList.begin(); it != exprList.end(); it++){
        evalExpList->push_back((**it).evaluate(context));
    }

    NExpressionList* evalNExpr = new NExpressionList(*evalExpList);
    return evalNExpr;

}

NExpression* NIdentifier::evaluate(CodeExecutionContext& context){
    //Busco el valor de la variable en el contexto.
    DEBUG_PRINT(("NIdentifier::evaluate\n"));
    //Tengo que obtener el valor del identificador.
    if (name.compare("print") == 0) {
        return this;
    } else {
        return context.getVariable(name)->evaluate(context);
    }
}

NExpression* NFunctionCall::evaluate(CodeExecutionContext& context){
    DEBUG_PRINT((BOLDBLACK"NFunctionCall::evaluate\n"RESET));
    /*
        Una funcionCall consiste de:
        NIdentifier& id;
        ExpressionList arguments;

        La idea es buscar la funcion `id` en el contexto, 
        evaluar (si es necesario) las expressiones en los argumentos.
        Mapear cada argumento a los definidos en la lista de identificadores
        de la Function.
        Crear un CodeExecutionBlock, guardar las variables 
        en el CodeExecutionBlock.
        Agregarlo al stack
        y hacer runCode de el block de la funcion.
        Esto devuelve una Expression (o mas bien ExpressionList)
        la cual se devuelve.
    */

    string funcName = id.name;

    string tablePrefix = "table.";
    string mathPrefix = "math.";

    if (funcName.compare("print") == 0){
        context.print(arguments);
        return new NNil();
    } else if (funcName.substr(0, tablePrefix.size()).compare(tablePrefix) == 0){
        DEBUG_PRINT((BOLDGREEN"Se corre funcion de la libreria table: %s\n"RESET, funcName.c_str()));
        string tableMethod = funcName.substr(tablePrefix.size(), funcName.size());
        NExpression* tableRes = context.table(tableMethod, arguments);
        //cout << "ERROR: Operaciones `table` todavia no estan implementadas.\n";
        return tableRes;
        
    } else if (funcName.substr(0, mathPrefix.size()).compare(mathPrefix) == 0){
        DEBUG_PRINT((BOLDGREEN"Se corre funcion de la libreria math: %s\n"RESET, funcName.c_str()));
        // Los argumentos de la variable de math hay que
        string mathMethod = funcName.substr(mathPrefix.size(), funcName.size());
        NExpression* mathRes = context.math(mathMethod, arguments);
        return mathRes;
    }

    NFunctionDeclaration* funcDecl = context.getFunction(id.name);

    if (funcDecl == 0){
        // No existe funcion, error.
        cout << "ERROR: Invalid function name " << id.name << endl;
        exit(0);
    }

    CodeExecutionBlock* funcBlock = new CodeExecutionBlock(funcDecl->block);
    context.push_block(funcBlock);

    // Tengo que mapear los arguments (NExpression) en this->arguments
    // con los de funcDecl->arguments (NIdentifier).

    // Si se reciben menos argumentos que los declarados, estos se pasan como nil
    // Si se reciben mas argumentos que los declarados, estos son ignorados.
    // Todos las variables que se agreguen se hace como locales.

    int var_count = 0;
    IdentifierList::const_iterator id_it;
    ExpressionList::const_iterator exp_it = arguments.begin();
    for (id_it = funcDecl->arguments.begin(); id_it != funcDecl->arguments.end(); id_it++){
        var_count++;
        string varName = (**id_it).name;
        NExpression* varExpression;
        if (exp_it != arguments.end()){
            // Si es una funcion entonces tengo que evaluarla.
            // y si es una variable tengo que obtener su valor.
            varExpression = (*exp_it)->evaluate(context);
        } else {
            varExpression = new NNil();
        }
        // Se agrega la variable al contexto como local.
        context.addVariable(varName, varExpression, 1);

        if (exp_it != arguments.end()){
            exp_it++;
        }
    }

    // Con las variables y el bloque en el contexto. Corro el bloque.
    DEBUG_PRINT((GREEN"Se cargan %d variables, se ejecuta el bloque.\n"RESET, var_count));
    NExpression* res = funcDecl->block.runCode(context);
    DEBUG_PRINT((GREEN"Se termina de ejecutar bloque.\n"RESET));
    DEBUG_PRINT((MAGENTA"Se tiene resultado de tipo: %s. Se evalua.\n"RESET, res->type_str().c_str()));
    res = res->evaluate(context);
    DEBUG_PRINT((MAGENTA"Res evaluado, ahora es de tipo: %s.\n"RESET, res->type_str().c_str()));

    // Quito el bloque de la funcion del contexto.
    context.pop_block();

    return res;
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

        /*//* Si es un NExpressionList entonces solo se usa el primero.
        if (rtype == EXPRESSION_LIST) {
            NExpressionList* nExprList = dynamic_cast<NExpressionList*>(&lhs);
            rEvalExpression = nExprList->exprList[0];
            rtype = rEvalExpression->type();
        }*/

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
    } else if (op == TK_OP_MIN_EQUALS || op == TK_OP_GRT_EQUALS || op == TK_OP_MIN || op == TK_OP_GRT) {
        /*
            Al igual que con TK_OP_MIN_EQUALS y TK_OP_GRT_EQUALS
            cuando tenemos TK_OP_GRT invertimos de lugar para 
            no repetir logica.

            left >= right
            right >= left
        */
        NExpression* lefths;
        NExpression* righths;

        if (op == TK_OP_GRT || op == TK_OP_GRT_EQUALS){
            // Invierto los operadores de lugar.
            lefths = &rhs;
            righths = &lhs;
        } else {
            // caso normal TK_OP_MIN
            lefths = &lhs;
            righths = &rhs;
        }

        double leftVal;
        int leftIsNum = 0;
        int leftIsStr = 0;
        try{
            leftVal = expressionToDouble(lefths);
            leftIsNum = 1;
        } catch (int e) {
            if (lefths->type() != STRING){
                cout << "ERROR: Invalid value for comparison..\n";
                exit(0);
            }
            leftIsStr = 1;
        }

        double rightVal;
        int rightIsNum = 0;
        int rightIsStr = 0;
        try{
            rightVal = expressionToDouble(righths);
            rightIsNum = 1;
        } catch (int e) {
            if (righths->type() != STRING){
                cout << "ERROR: Invalid value for comparison..\n";
                exit(0);
            }
            rightIsStr = 1;
        }

        if (rightIsNum != leftIsNum || rightIsStr != leftIsStr){
            cout << "ERROR: Invalid combination of values for comparison..\n";
            exit(0);
        }

        if (rightIsStr == 1) {
            // Comparo strings.
            string rightStr = expressionToString(righths);
            string leftStr = expressionToString(lefths);

            int compareStr = rightStr.compare(leftStr);
            int trueVal = 0;

            if (((op == TK_OP_MIN || op == TK_OP_GRT) && compareStr > 0) || ((op == TK_OP_MIN_EQUALS || op == TK_OP_GRT_EQUALS) && compareStr >= 0)){
                trueVal = 1;
            }
            NBoolean* resExpr = new NBoolean(trueVal);
            return resExpr;
        }

        if (rightIsNum == 1) {
            int trueVal = 0;

            if (((op == TK_OP_MIN || op == TK_OP_GRT) && leftVal < rightVal) || ((op == TK_OP_MIN_EQUALS || op == TK_OP_GRT_EQUALS) && leftVal <= rightVal)) {
                trueVal = 1;
            }
            NBoolean* resExpr = new NBoolean(trueVal);
            return resExpr;
        }

        return new NNil();

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
            rDoubleValue = -rDoubleValue;
        } else if (rtype == DOUBLE) {
            // Si left es double entonces right lo va a ser tambien (por que int int ya termino.)
            rDoubleValue = (dynamic_cast<NDouble*>(rEvalExpression))->value;
            rDoubleValue = -rDoubleValue;
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

NExpression* NTableFieldSingleExpression::evaluate(CodeExecutionContext& context){ }
NExpression* NTableFieldIdentifier::evaluate(CodeExecutionContext& context){ }
NExpression* NTableFieldExpression::evaluate(CodeExecutionContext& context){ }
NExpression* NTableFieldList::evaluate(CodeExecutionContext& context){ }
NExpression* NTable::evaluate(CodeExecutionContext& context){
    /*
    Las NTable estan compuestas por un NTableFieldList
    Este tiene una lista de tipo TableFieldList que 
    puede tener elementos de tipo:
        NTableFieldSingleExpression
        NTableFieldIdentifier
        NTableFieldExpression

    La idea en evaluarla es pasar todos los tipos a:
    NTableFieldExpression
     (compuesto de keyExpr y valExpr, con Expressiones finales (sin evaluacion)).

    Esto se debera ejecutar en la definicion de la NTable como en la evaluacion
    subsecuente.

    Idealmente luego de la inicializacion no se deberia de tener que cambiar los tipos
    (a NTableFieldExpression) pero puede llegar pasar.

    La idea es recorrer la lista, manteniendo un contador para los campos que no tienen key.
    Y evaluando las expressiones agregandolas en un mapa.
    
    En lo ideal se tendria un mapa en vez de una lista, pero como la implementacion permite claves
    variadas para no complicarla implementados las primitivas de mapas a partir de la lista.

    En el caso de tener claves repetidas se sobreescriben.

    */

    int countSinKey = 1; // Los que no tienen indice se empiezan a contar desde 1.
    TableFieldList* normalTableFieldList = new TableFieldList();

    GenericTableFieldList::const_iterator tf_it;
    for (tf_it = fieldList.fieldList.begin(); tf_it != fieldList.fieldList.end(); tf_it++) {
        if ((*tf_it)->type() == TABLE_FIELD_SINGLE_EXPRESSION) {

            NTableFieldSingleExpression* tablFieldSingleExpr = dynamic_cast<NTableFieldSingleExpression*>(*tf_it);

            NExpression* intKey = new NInteger(countSinKey);
            NExpression* valExpr = tablFieldSingleExpr->expression.evaluate(context);
            NTableFieldExpression* evaluated = new NTableFieldExpression(*intKey, *valExpr, true);

            normalTableFieldList->push_back(evaluated);
            countSinKey++;

        } else if ((*tf_it)->type() == TABLE_FIELD_IDENTIFIER) {

            NTableFieldIdentifier* tablFieldIdenExpr = dynamic_cast<NTableFieldIdentifier*>(*tf_it);

            // Se busca el Identifier en el contexto, y se evalua.

            NExpression* keyExpr = tablFieldIdenExpr->identifier.evaluate(context);
            NExpression* valExpr = tablFieldIdenExpr->expression.evaluate(context);
            NTableFieldExpression* evaluated = new NTableFieldExpression(*keyExpr, *valExpr, false);

            normalTableFieldList->push_back(evaluated);

        } else if ((*tf_it)->type() == TABLE_FIELD_LIST) {

            NTableFieldExpression* tablFieldExpr = dynamic_cast<NTableFieldExpression*>(*tf_it);

            NExpression* keyExpr = tablFieldExpr->keyExpr.evaluate(context);
            NExpression* valExpr = tablFieldExpr->valExpr.evaluate(context);
            NTableFieldExpression* evaluated = new NTableFieldExpression(*keyExpr, *valExpr, false);

            normalTableFieldList->push_back(evaluated);

        } else {
            cout << "ERROR: Invalid expression in table evaluation.\n";
            exit(0);
        }
    }

    //TODO: Falta no agregar duplicados.
    NTableExpr* evalTable = new NTableExpr();
    evalTable->fieldList = normalTableFieldList;
    return evalTable;
}

void NTableExpr::add_field(NExpression* keyExpr, NExpression* valExpr, bool auto_incremented) {
    remove_field(keyExpr);
    NTableFieldExpression* fieldExpr = new NTableFieldExpression(*keyExpr, *valExpr, auto_incremented);
    fieldList->push_back(fieldExpr);
}

void NTableExpr::add_field(NExpression* valExpr) {
    // Sin key, se utiliza el counter.
    NInteger* keyExpr = new NInteger(counter);
    add_field(keyExpr, valExpr, true);
    counter++;
}

void NTableExpr::remove_field(NExpression* keyExpr){
    TableFieldList::iterator field_it;
    for (field_it = fieldList->begin(); field_it != fieldList->end();) {
        NTableFieldExpression* fieldExpr = dynamic_cast<NTableFieldExpression*>(*field_it);
        if (fieldExpr->keyExpr.type() != keyExpr->type()) {
            ++field_it;
        } else if (keyExpr->type() == STRING && expressionToString(keyExpr).compare(expressionToString(&fieldExpr->keyExpr)) != 0) {
            ++field_it;
        } else if (keyExpr->type() == BOOLEAN && expressionToBoolean(keyExpr) != expressionToBoolean(&fieldExpr->keyExpr)) {
            ++field_it;
        } else if ((keyExpr->type() == INTEGER || keyExpr->type() == DOUBLE) && expressionToDouble(keyExpr) != expressionToDouble(&fieldExpr->keyExpr)) {
            ++field_it;
        } else {
            field_it = fieldList->erase(field_it);
        }
    }
}

void NTableExpr::sort_fields() {
    // Asumo que es una table con indices enteros y 
    sort(fieldList->begin(), fieldList->end(), compareTableFields);
    // Itero insertando con indices
    TableFieldList* sorted = new TableFieldList();
    TableFieldList::iterator field_it;
    int counter = 1;
    for (field_it = fieldList->begin(); field_it != fieldList->end(); field_it++) {
        if ((*field_it)->auto_incremented) {
            NExpression* keyExpr = new NInteger(counter);
            NTableFieldExpression* fieldExpr = new NTableFieldExpression(*keyExpr, (*field_it)->valExpr, true);
            sorted->push_back(fieldExpr);
            counter++;
        } else {
            NTableFieldExpression* fieldExpr = new NTableFieldExpression((*field_it)->keyExpr, (*field_it)->valExpr, false);
            sorted->push_back(fieldExpr);
        }
    }
    fieldList = sorted;
}

NExpression* NBlock::evaluate(CodeExecutionContext& context){
    
}

NExpression* NAnonFunctionDeclaration::evaluate(CodeExecutionContext& context){
    
}



double expressionToDouble(NExpression* expr){
    double doubleVal;
    if (expr->type() == STRING) {
        // Primero tratamos a double.
        istringstream ss((dynamic_cast<NString*>(expr))->value);
        if (!(ss >> doubleVal)){
            // No es double.
            throw 1;
        }
    } else if (expr->type() == INTEGER) {
        NInteger* intExpression = dynamic_cast<NInteger*>(expr);
        doubleVal = (double) intExpression->value;
    } else if (expr->type() == DOUBLE) {
        NDouble* doubExpression = dynamic_cast<NDouble*>(expr);
        doubleVal = doubExpression->value;
    } else {
        throw 1;
    }
    return doubleVal;
}

int expressionToBoolean(NExpression* expr) {
    // Es 0 solo si es false o nil
    if (expr->type() == BOOLEAN) {
        NBoolean* boolean = dynamic_cast<NBoolean*>(expr);
        // Si es False entonces True.
        return boolean->trueVal;
    }

    if (expr->type() == NIL) {
        return 0;
    }

    // Else devuelvo 1.
    return 1;
}

string expressionToString(NExpression* expr) {
    if (expr->type() == STRING) {
        NString* strExpr = dynamic_cast<NString*>(expr);
        return strExpr->value;
    }
}

bool compareTableFields(NTableFieldExpression* a, NTableFieldExpression* b) {
    DEBUG_PRINT((RED"Comparo con compareTableFields\n"RESET));
    // a < b
    if (a->valExpr.type() == STRING && (b->valExpr.type() == INTEGER || b->valExpr.type() == DOUBLE)){
        DEBUG_PRINT((RED"El primero es string devuelvo false.\n"RESET));
        return false;
    }

    if ((a->valExpr.type() == INTEGER || a->valExpr.type() == DOUBLE) && (b->valExpr.type() == INTEGER || b->valExpr.type() == DOUBLE)){
        
        double aDouble = expressionToDouble(&a->valExpr);
        double bDouble = expressionToDouble(&b->valExpr);
        DEBUG_PRINT((BOLDRED"Comparo los valores a = %f, b = %f\n"RESET, aDouble, bDouble));
        return aDouble < bDouble;
    }

    DEBUG_PRINT((RED"Comparacion por default\n"RESET));

    return true;
}
