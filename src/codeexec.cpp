#include "node.h"
#include "codeexec.h"
#include "parser.hpp" /* para tener acceso al enum de tokens */
#define _USE_MATH_DEFINES
#include "math.h"

using namespace std;

void* CodeExecutionContext::executeCode(NBlock& root){

    /*
        Se crea un nuevo CodeExecutionBlock y en el top del stack blocks.
    */
    CodeExecutionBlock* execBlock = new CodeExecutionBlock(root);
    this->blocks.push_back(execBlock);

    /*
        Run
    */
    DEBUG_PRINT(("Se corre el codigo. Size del stack: %d\n", (int) this->blocks.size()));

    /*
        Un bloque esta compuesto de una lista de statements y un last statement.
    */
    root.runCode(*this);

    DEBUG_PRINT(("Se corrio el codigo\n"));
    this->blocks.pop_back();
}

void CodeExecutionContext::push_block(CodeExecutionBlock* ceb){
    this->blocks.push_back(ceb);
}
void CodeExecutionContext::pop_block(){
    this->blocks.pop_back();
}

NExpression* CodeExecutionContext::getVariable(string name){
    vector<CodeExecutionBlock*>::reverse_iterator it;
    map<string, NExpression*>::iterator var_it;
    for (it = blocks.rbegin(); it != blocks.rend(); ++it){
        // Busco si la funcion de nombre `name` esta definida en el mapa de funciones.
        var_it = (**it).variables.find(name);
        if (var_it != (**it).variables.end()){
            return var_it->second;
        }
    }
};

NFunctionDeclaration* CodeExecutionContext::getFunction(string name){
    vector<CodeExecutionBlock*>::reverse_iterator it;
    map<string, NFunctionDeclaration*>::iterator func_it;
    for (it = blocks.rbegin(); it != blocks.rend(); ++it){
        // Busco si la funcion de nombre `name` esta definida en el mapa de funciones.
        func_it = (**it).functions.find(name);
        if (func_it != (**it).functions.end()){
            return func_it->second;
        }
    }
}

void CodeExecutionContext::addFunction(string name, NFunctionDeclaration* function){
    // Obtengo el bloque actual de ejecucion.
    /*
        A diferencia de las variables las funciones siempre se guardan en el ultimo bloque.
        Y se sobreescriben.
    */
    CodeExecutionBlock* block = blocks.back();
    block->functions[name] = function;
}

void CodeExecutionContext::addVariable(string name, NExpression* expression, int isLocal){
    CodeExecutionBlock* block;
    if (isLocal) {
        // Si es local entonces la guardo en el ultimo bloque.
        block = blocks.back();
        DEBUG_PRINT(("Se inserta la variable %s en el ultimo bloque\n", name.c_str()));
    } else {
        // Busco la variable en la lista de bloques de atras para adelante, si la encuentro entonces la sobreescribo.
        // Si no la encuentro entonces guardo como si fuera local.
        int seEncuentra = 0;
        vector<CodeExecutionBlock*>::reverse_iterator it;
        for (it = blocks.rbegin(); it != blocks.rend(); ++it){
            DEBUG_PRINT((BLUE"Se busca la variable en block\n"RESET));
            // Busco si la funcion de nombre `name` esta definida en el mapa de funciones.
            if ((**it).variables.find(name) != (**it).variables.end()){
                DEBUG_PRINT(("Se encuentra la variable %s en otro bloque.\n", name.c_str()));
                block = (*it);
                seEncuentra = 1;
                break;
            }
        }

        if (seEncuentra == 0) {
            DEBUG_PRINT((BLUE"No se encuentra, se inserta en el ultimo bloque.\n"RESET));
            block = blocks.back();
        }
    }
    DEBUG_PRINT((GREEN"Se inserta la variable: "RESET)); DEBUG_PRINT((RED"%s\n"RESET, name.c_str()));
    block->variables[name] = expression;
    DEBUG_PRINT((GREEN"Variable "RESET)); DEBUG_PRINT((RED"%s"RESET, name.c_str())); DEBUG_PRINT((GREEN" insertada.\n"RESET)); 
}


int CodeExecutionContext::functionDefined(string name){
    /*
    Itero por los blockes del contexto buscando en la lista de funciones
    de cada uno para saber si esta definida.
    */
    vector<CodeExecutionBlock*>::iterator it;
    for (it = blocks.begin(); it != blocks.end(); it++){
        // Busco si la funcion de nombre `name` esta definida en el mapa de funciones.
    }
}

NExpression* CodeExecutionContext::math(string method, ExpressionList exprList) {
    DEBUG_PRINT(("Library MATH, metodo: %s ->\n", method.c_str()));
    DEBUG_PRINT((BLUE"Se tienen %d expressions para evaluar.\n"RESET, (int) exprList.size()));
    // Paso los argumentos de exprList a un vector de numeros.
    vector<double> doubleArgs;

    ExpressionList* exprListExpanded = expandExpressionList(exprList);
    ExpressionList::const_iterator exp_it;

    for (exp_it = exprListExpanded->begin(); exp_it != exprListExpanded->end(); exp_it++){
        try {
            // trato de pasar expression a double, si da error entonces termino el programa.
            double arg = expressionToDouble(*exp_it);
            doubleArgs.push_back(arg);
        } catch (int e) {
            int it_pos = exp_it - exprListExpanded->begin();
            cout << "ERROR: bad argument #" << it_pos << " to '" << method << "' (number expected)\n";
            exit(0);
        }
    }

    DEBUG_PRINT((RED"Tengo argumentos double, size: %d, first: %f\n"RESET, (int) doubleArgs.size(), doubleArgs.front()));
    double doubleRes;

    /*
    math.abs (x)
    math.acos (x)
    math.asin (x)
    math.atan (x)
    math.ceil (x)
    math.cos (x)
    math.exp (x)
    math.floor (x)
    math.log (x)
    math.log10 (x)
    math.max (x, ···)
    math.min (x, ···)
    math.pi
    math.pow (x, y)
    math.random ([m [, n]])
    math.sin (x)
    math.sqrt (x)
    math.tan (x)
    */
    if (method.compare("abs") == 0) {
        if (doubleArgs.size() < 1) {
            cout << "ERROR: bad argument #1 to 'abs' (number expected, got no value)\n";
            exit(0);
        }
        doubleRes = fabs(doubleArgs.front());
    } else if (method.compare("acos") == 0) {
        if (doubleArgs.size() < 1) {
            cout << "ERROR: bad argument #1 to 'acos' (number expected, got no value)\n";
            exit(0);
        }
        doubleRes = acos(doubleArgs.front());
    } else if (method.compare("asin") == 0) {
        if (doubleArgs.size() < 1) {
            cout << "ERROR: bad argument #1 to 'asin' (number expected, got no value)\n";
            exit(0);
        }
        doubleRes = asin(doubleArgs.front());
    } else if (method.compare("atan") == 0) {
        if (doubleArgs.size() < 1) {
            cout << "ERROR: bad argument #1 to 'atan' (number expected, got no value)\n";
            exit(0);
        }
        doubleRes = atan(doubleArgs.front());
    } else if (method.compare("ceil") == 0) {
        if (doubleArgs.size() < 1) {
            cout << "ERROR: bad argument #1 to 'ceil' (number expected, got no value)\n";
            exit(0);
        }
        doubleRes = ceil(doubleArgs.front());
    } else if (method.compare("cos") == 0) {
        if (doubleArgs.size() < 1) {
            cout << "ERROR: bad argument #1 to 'cos' (number expected, got no value)\n";
            exit(0);
        }
        doubleRes = cos(doubleArgs.front());
    } else if (method.compare("exp") == 0) {
        if (doubleArgs.size() < 1) {
            cout << "ERROR: bad argument #1 to 'exp' (number expected, got no value)\n";
            exit(0);
        }
        doubleRes = exp(doubleArgs.front());
    } else if (method.compare("floor") == 0) {
        if (doubleArgs.size() < 1) {
            cout << "ERROR: bad argument #1 to 'floor' (number expected, got no value)\n";
            exit(0);
        }
        doubleRes = floor(doubleArgs.front());
    } else if (method.compare("log") == 0) {
        if (doubleArgs.size() < 1) {
            cout << "ERROR: bad argument #1 to 'log' (number expected, got no value)\n";
            exit(0);
        }
        doubleRes = log(doubleArgs.front());
    } else if (method.compare("log10") == 0) {
        if (doubleArgs.size() < 1) {
            cout << "ERROR: bad argument #1 to 'log10' (number expected, got no value)\n";
            exit(0);
        }
        doubleRes = log10(doubleArgs.front());
    } else if (method.compare("max") == 0) {
        if (doubleArgs.size() < 1) {
            cout << "ERROR: bad argument #1 to 'max' (number expected, got no value)\n";
            exit(0);
        }
        vector<double>::iterator result;
        result = max_element(doubleArgs.begin(), doubleArgs.end());
        doubleRes = *result;
    } else if (method.compare("min") == 0) {
        if (doubleArgs.size() < 1) {
            cout << "ERROR: bad argument #1 to 'min' (number expected, got no value)\n";
            exit(0);
        }
        vector<double>::iterator result;
        result = min_element(doubleArgs.begin(), doubleArgs.end());
        doubleRes = *result;
    } else if (method.compare("pi") == 0) {
        doubleRes = M_PI;
    } else if (method.compare("pow") == 0) {
        if (doubleArgs.size() < 2) {
            if (doubleArgs.size() < 1)
                cout << "ERROR: bad argument #1 to 'pow' (number expected, got no value)\n";
            else
                cout << "ERROR: bad argument #2 to 'pow' (number expected, got no value)\n";

            exit(0);
        }
        doubleRes = pow(doubleArgs[0], doubleArgs[1]);
    } else if (method.compare("random") == 0) {
        if (doubleArgs.size() == 0) {
            doubleRes = ((double) rand() / (RAND_MAX));
        } else if (doubleArgs.size() == 1) {
            doubleRes = rand() % (int) doubleArgs.front() + 1;
        } else {
            doubleRes = rand() % (int) doubleArgs[1] +  (int) doubleArgs[0];
        }
    } else if (method.compare("sin") == 0) {
        if (doubleArgs.size() < 1) {
            cout << "ERROR: bad argument #1 to 'sin' (number expected, got no value)\n";
            exit(0);
        }
        doubleRes = sin(doubleArgs.front());
    } else if (method.compare("sqrt") == 0) {
        if (doubleArgs.size() < 1) {
            cout << "ERROR: bad argument #1 to 'sqrt' (number expected, got no value)\n";
            exit(0);
        }
        doubleRes = sqrt(doubleArgs.front());
    } else if (method.compare("tan") == 0) {
        if (doubleArgs.size() < 1) {
            cout << "ERROR: bad argument #1 to 'tan' (number expected, got no value)\n";
            exit(0);
        }
        doubleRes = tan(doubleArgs.front());
    } else {
        cout << "ERROR: Libreria math." << method << " no esta implementada\n";
        exit(0);
    }

    NExpression* resExpression = new NDouble(doubleRes);
    return resExpression;
}

NExpression* CodeExecutionContext::table(string method, ExpressionList exprList) {
    /*
        El primer valor de la expression list siempre es la tabla que se quiere manipular.
    */
    ExpressionList* exprListExpanded = expandExpressionList(exprList);
    if (exprListExpanded->front()->type() != TABLE_EXPRESSION) {
        cout << "ERROR: bad argument #1 to '" << method << "' (table expected)\n";
        exit(0);
    }

    NTableExpr* tableExpr = dynamic_cast<NTableExpr*>(exprListExpanded->front());

    if (method.compare("foreach") == 0) {
        DEBUG_PRINT((BOLDGREEN"Se aplica la operacion table.foreach.\n"RESET));
        if (exprListExpanded->size() < 2) {
            cout << "ERROR: Invalid number of params in table.foreach.\n";
            exit(0);
        }
        ExpressionList::iterator expr_it = exprListExpanded->begin();
        expr_it++;
        NIdentifier* functionIdentifier = dynamic_cast<NIdentifier*>(*expr_it);
        if (functionIdentifier->name.compare("print") == 0) {
            DEBUG_PRINT((BOLDRED"Print de table.\n"RESET));
            TableFieldList::iterator field_it;
            for (field_it = tableExpr->fieldList->begin(); field_it != tableExpr->fieldList->end(); field_it++) {
                DEBUG_PRINT((BOLDBLUE"Print de field de table tipo: %s\n"RESET, (*field_it)->type_str().c_str()));
                NTableFieldExpression* fieldExpr = dynamic_cast<NTableFieldExpression*>(*field_it);
                ExpressionList* asExprList = new ExpressionList();
                DEBUG_PRINT((BOLDBLUE"fieldExpr->keyExpr.type() = %s, fieldExpr->valExpr.type() = %s, \n"RESET, fieldExpr->keyExpr.type_str().c_str(), fieldExpr->valExpr.type_str().c_str()));
                asExprList->push_back(fieldExpr->keyExpr.evaluate(*this));
                asExprList->push_back(fieldExpr->valExpr.evaluate(*this)); 
                print(*asExprList);
            }
            DEBUG_PRINT((BOLDRED"Se termina de imprimir table.\n"RESET));
        }

    } else if (method.compare("insert") == 0) {
        DEBUG_PRINT((BOLDGREEN"Se aplica la operacion table.insert.\n"RESET));
        if (exprListExpanded->size() < 2) {
            cout << "ERROR: Invalid number of params in table.insert.\n";
            exit(0);
        }
        ExpressionList::iterator expr_it = exprListExpanded->begin();
        expr_it++;
        NExpression* valExpr = *expr_it;
        tableExpr->add_field(valExpr);

    } else if (method.compare("sort") == 0) {
        tableExpr->sort_fields();
    }
    return new NNil();
}

void CodeExecutionContext::print(ExpressionList exprList){
    DEBUG_PRINT(("FUNCION PRINT ->\n"));
    DEBUG_PRINT((GREEN"Se tienen %d expressions para imprimir\n"RESET, (int) exprList.size()));

    // Creo una nueva ExpressionList con las expressiones evaluadas (si es necesario).
    ExpressionList* exprListFinal = expandExpressionList(exprList);

    ExpressionList::const_iterator exp_it;
    ExpressionList::const_iterator final_it = exprListFinal->end();
    --final_it;

    for (exp_it = exprListFinal->begin(); exp_it != exprListFinal->end(); exp_it++){
        this->print_expr((*exp_it));
        if (exp_it != final_it){
            cout << "\t";
        }
    } // endfor
    cout << endl;
    DEBUG_PRINT(("\n <- END FUNCION PRINT\n"));
}

void CodeExecutionContext::print_expr(NExpression* expr){
    switch (expr->type()){
        case NIL:
            cout << "nil";
            break;
        case BOOLEAN:
            if ((dynamic_cast<NBoolean*>(expr))->trueVal == 1){
                cout << "true";
            } else {
                cout << "false";
            }
            break;
        case INTEGER:
            cout << (dynamic_cast<NInteger*>(expr))->value;
            break;
        case DOUBLE:
            cout.precision(14);
            cout << (dynamic_cast<NDouble*>(expr))->value;
            break;
        case STRING: {
                string value = (dynamic_cast<NString*>(expr))->value;
                cout << value;
            }
            break;
        case IDENTIFIER: {
                NIdentifier* ident = dynamic_cast<NIdentifier*>(expr);
                NExpression* idExp = ident->evaluate(*this);
                if (idExp != 0){
                    this->print_expr(idExp);
                } else {
                    cout << "ERROR: Variable " << ident->name << " no existe." << endl;
                    exit(0);
                }
            }
            break;
        case FUNCTION_CALL: {
                NFunctionCall* funcCall = dynamic_cast<NFunctionCall*>(expr);
                this->print_expr(funcCall->evaluate(*this));
            }
            break;
        case BINARY_OPERATOR: {
                DEBUG_PRINT((YELLOW"Se va a imprimir una expresion binaria\n"RESET));
                NBinaryOperator* binOp = dynamic_cast<NBinaryOperator*>(expr);
                NExpression* val = binOp->evaluate(*this);
                this->print_expr(val);
            }
            break;
        case UNARY_OPERATOR:{
                DEBUG_PRINT((YELLOW"Se va a imprimir una expresion unaria\n"RESET));
                NUnaryOperator* unOp = dynamic_cast<NUnaryOperator*>(expr);
                NExpression* val = unOp->evaluate(*this);
                this->print_expr(val);
            }
            break;
        case BLOCK:
            cout << "ERROR: Print de block NOT IMPLEMENTED" << endl;
            break;
        case ANON_FUNCTION_DECLARATION:
            cout << "ERROR: Print de anon function declaration NOT IMPLEMENTED" << endl;
            break;
        case EXPRESSION_LIST: {
                // TODO: Imprimir dependiendo de como se llama la funcion.
                NExpressionList* exprList = dynamic_cast<NExpressionList*>(expr);
                this->print_expr(exprList->exprList[0]);
            }
            break;
        case TABLE_EXPRESSION:
            // Imprimo la direccion de puntero.
            cout << "table: " << expr;
            break;
        default:
            cout << "ERROR: PRINT DEFAULT ERROR!!" << endl;
            exit(1);
    }
}

void CodeExecutionContext::printFunctionsAndVariables(){
    int counter = 0;
    vector<CodeExecutionBlock*>::reverse_iterator it;
    for (it = blocks.rbegin(); it != blocks.rend(); it++){
        // Busco si la funcion de nombre `name` esta definida en el mapa de funciones.
        DEBUG_PRINT(("Bloque: %d\n", counter++));
        for (map<string, NFunctionDeclaration*>::iterator it2=(**it).functions.begin(); it2!=(**it).functions.end(); ++it2){
            DEBUG_PRINT(("Function: %s => ...\n", it2->first.c_str()));
        }
        for (map<string, NExpression*>::iterator it2=(**it).variables.begin(); it2!=(**it).variables.end(); ++it2){
            DEBUG_PRINT(("Expression: %s => ...\n", it2->first.c_str()));
        }
    }
}


ExpressionList* CodeExecutionContext::expandExpressionList(ExpressionList exprList) {
    ExpressionList* expanded = new ExpressionList();
    ExpressionList::const_iterator exp_it;
    for (exp_it = exprList.begin(); exp_it != exprList.end(); exp_it++) {
        NExpression* evaluatedExpression = (*exp_it)->evaluate(*this);
        if (evaluatedExpression->type() == EXPRESSION_LIST) {
            ExpressionList::const_iterator exp_exp_list;
            NExpressionList* nExprList = dynamic_cast<NExpressionList*>(evaluatedExpression);
            for (exp_exp_list = nExprList->exprList.begin(); exp_exp_list != nExprList->exprList.end(); exp_exp_list++) {
                expanded->push_back(*exp_exp_list);
            }
        } else {
            expanded->push_back(evaluatedExpression);
        }
    }
    return expanded;
}