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

NExpression* CodeExecutionContext::getVariable(string name){
    vector<CodeExecutionBlock*>::reverse_iterator it;
    map<string, NExpression*>::iterator var_it;
    for (it = blocks.rbegin(); it != blocks.rend(); ++it){
        // Busco si la funcion de nombre `name` esta definida en el mapa de funciones.
        var_it = (**it).variables.find(name);
        if (var_it != (**it).variables.end()){
            cout << "CodeExecutionContext::getVariable " << name << endl;;
            return var_it->second;
        }
    }
};

void CodeExecutionContext::addFunction(string name, NFunctionDeclaration* function){
    // Obtengo el bloque actual de ejecucion.
    /*
        A diferencia de las variables las funciones siempre se guardan en el ultimo bloque.
        Y se sobreescriben.
    */
    CodeExecutionBlock* block = blocks.back();
    block->functions.insert(make_pair(name, function));
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
        vector<CodeExecutionBlock*>::reverse_iterator it;
        for (it = blocks.rbegin(); it != blocks.rend(); ++it){
            // Busco si la funcion de nombre `name` esta definida en el mapa de funciones.
            if ((**it).variables.find(name) != (**it).variables.end()){
                cout << "Se encuentra la variable " << name << " en otro bloque.\n";
                block = (*it);
                break;
            }
        }

        if (!block){
            // Si no la encuentro devuelvo el local.
            block = blocks.back();
        }
    }
    block->variables.insert(make_pair(name, expression));
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

    ExpressionList::const_iterator exp_it;
    ExpressionList::const_iterator final_it = exprList.end();
    --final_it;
    for (exp_it = exprList.begin(); exp_it != exprList.end(); exp_it++){
        this->print_expr((*exp_it));
        // TODO: Si no es la ultima, imprimo 1 tab.
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
                cout << value.substr(1, value.size()-2);
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
        case FUNCTION_CALL:
            cout << "FUNCTION_CALL -> NOT IMPLEMENTED" << endl;
            break;
        case BINARY_OPERATOR: {
                NBinaryOperator* binOp = dynamic_cast<NBinaryOperator*>(expr);
                NExpression* val = binOp->evaluate(*this);
                this->print_expr(val);
            }
            break;
        case UNARY_OPERATOR:
            cout << "UNARY_OPERATOR -> NOT IMPLEMENTED" << endl;
            break;
        case BLOCK:
            cout << "BLOCK -> NOT IMPLEMENTED" << endl;
            break;
        case ANON_FUNCTION_DECLARATION:
            cout << "ANON_FUNCTION_DECLARATION -> NOT IMPLEMENTED" << endl;
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
        cout << "Bloque " << counter << ":\n";
        for (map<string, NFunctionDeclaration*>::iterator it2=(**it).functions.begin(); it2!=(**it).functions.end(); ++it2){
            cout << "Function: " << it2->first << " => " << it2->second << '\n';
        }
        for (map<string, NExpression*>::iterator it2=(**it).variables.begin(); it2!=(**it).variables.end(); ++it2){
            cout << "Expression: " << it2->first << " => " << it2->second << '\n';
        }
    }
}