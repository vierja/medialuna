#include "node.h"
#include "codeexec.h"
#include "parser.hpp" /* para tener acceso al enum de tokens */

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
    cout << "Se corre el codigo. Size del stack: " << this->blocks.size() << endl;

    /*
        Un bloque esta compuesto de una lista de statements y un last statement.
    */
    root.runCode(*this);

    cout << "Se corrio el codigo" << endl;
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
        cout << "Se inserta la variable " << name << " en el ultimo bloque" << endl;
    } else {
        // Busco la variable en la lista de bloques de atras para adelante, si la encuentro entonces la sobreescribo.
        // Si no la encuentro entonces guardo como si fuera local.
        int seEncuentra = 0;
        vector<CodeExecutionBlock*>::reverse_iterator it;
        for (it = blocks.rbegin(); it != blocks.rend(); ++it){
            DEBUG_PRINT((BLUE"Se busca la variable en block\n"RESET));
            // Busco si la funcion de nombre `name` esta definida en el mapa de funciones.
            if ((**it).variables.find(name) != (**it).variables.end()){
                cout << "Se encuentra la variable " << name << " en otro bloque.\n";
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
    DEBUG_PRINT((GREEN"Se inserta la variable.\n"RESET));
    block->variables[name] = expression;
    DEBUG_PRINT((GREEN"Variable insertable.\n"RESET));
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

void CodeExecutionContext::print(ExpressionList exprList){
    cout << "FUNCION PRINT ->\n";
    DEBUG_PRINT((GREEN"Se tienen %d expressions para imprimir\n"RESET, (int) exprList.size()));
    ExpressionList* exprListFinal = new ExpressionList();

    // Creo una nueva ExpressionList con las expressiones evaluadas (si es necesario).
    ExpressionList::const_iterator exp_it;
    for (exp_it = exprList.begin(); exp_it != exprList.end(); exp_it++){
        NExpression* evaluatedExpression = (*exp_it)->evaluate(*this);
        exprListFinal->push_back(evaluatedExpression);
    }

    // Si solo se tiene una expression para imprimir
    // y esta es NExpressionList, imprimo cada una de esas.
    // si una de varias expressiones es NExpressionList entonces imprimo normal y uso la primera.
    if (exprListFinal->size() == 1){
        if (exprListFinal->front()->type() == EXPRESSION_LIST){
            NExpressionList* nExprList = dynamic_cast<NExpressionList*>(exprListFinal->front());
            exprListFinal = &nExprList->exprList;
        }
    }

    ExpressionList::const_iterator final_it = exprListFinal->end();
    --final_it;

    for (exp_it = exprListFinal->begin(); exp_it != exprListFinal->end(); exp_it++){
        this->print_expr((*exp_it));
        if (exp_it != final_it){
            cout << "\t";
        }
    } // endfor
    cout << "\n <- END FUNCION PRINT\n";
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
                    cout << "VARIABLE " << ident->name << " NO EXISTE." << endl;
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
            cout << "BLOCK -> NOT IMPLEMENTED" << endl;
            break;
        case ANON_FUNCTION_DECLARATION:
            cout << "ANON_FUNCTION_DECLARATION -> NOT IMPLEMENTED" << endl;
            break;
        case EXPRESSION_LIST: {
                // TODO: Imprimir dependiendo de como se llama la funcion.
                NExpressionList* exprList = dynamic_cast<NExpressionList*>(expr);
                this->print_expr(exprList->exprList[0]);
            }
            break;
        default:
            cout << "PRINT DEFAULT ERROR!!" << endl;
            exit(1);
    }
}

void CodeExecutionContext::printFunctionsAndVariables(){
    int counter = 0;
    vector<CodeExecutionBlock*>::reverse_iterator it;
    for (it = blocks.rbegin(); it != blocks.rend(); it++){
        // Busco si la funcion de nombre `name` esta definida en el mapa de funciones.
        cout << "Bloque " << counter++ << ":\n";
        for (map<string, NFunctionDeclaration*>::iterator it2=(**it).functions.begin(); it2!=(**it).functions.end(); ++it2){
            cout << "Function: " << it2->first << " => " << it2->second << '\n';
        }
        for (map<string, NExpression*>::iterator it2=(**it).variables.begin(); it2!=(**it).variables.end(); ++it2){
            cout << "Expression: " << it2->first << " => " << it2->second << '\n';
        }
    }
}