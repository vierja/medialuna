%error-verbose

%{
    #include "node.h"
    NBlock *programBlock;

    extern int yylex();
    void yyerror(const char *s) { printf("ERROR: %s\n", s); };
    #define YYDEBUG 1 /* For Debugging */
%}


%union {
    Node *node;
    NBlock *block;
    NExpression *expr;
    NStatement *stmt;
    NIdentifier *ident;
    NString *nstring;
    NLastStatement *laststat;
    NVariableDeclaration *var_decl;
    NTableFieldList* tablfields;
    NTableField* tablfield;
    NIfCond *ifcond;
    std::vector<NVariableDeclaration*> *varvec;
    std::vector<NExpression*> *exprvec;
    std::vector<NStatement*> *statvec;
    std::vector<NIdentifier*> *idvec;
    std::vector<NExpression*> *expvec;
    std::vector<NIfCond*> *condvec;
    std::string *string;
    int token;
}

%token <string> TK_ID TK_NUMBER_INT TK_NUMBER_DOUBLE TK_BOOLEAN TK_STRING
%token <token> TK_OP_PLUS TK_OP_MINUS TK_OP_TIMES TK_OP_DIVIDED TK_OP_MOD
%token <token> TK_OP_EXP TK_OP_HASH TK_OP_EQUALS TK_OP_DIFF TK_OP_MIN_EQUALS 
%token <token> TK_OP_GRT_EQUALS TK_OP_MIN TK_OP_GRT TK_OP_ASSIGN TK_OP_DOTDOT
%token <token> TK_KW_AND TK_KW_OR 
%token <token> TK_KW_NOT 
%token TK_OP_OPEN_PAREN
%token TK_OP_CLOS_PAREN
%token TK_OP_OPEN_BRACE
%token TK_OP_CLOS_BRACE
%token TK_OP_OPEN_BRACK
%token TK_OP_CLOS_BRACK
%token TK_OP_SEMICOLON
%token TK_OP_COLON
%token TK_OP_COMA
%token TK_OP_DOT
%token TK_OP_ELIPSIS
%token TK_KW_BREAK
%token TK_KW_DO
%token TK_KW_ELSE
%token TK_KW_ELSEIF
%token TK_KW_END
%token TK_KW_FOR
%token TK_KW_FUNCTION
%token TK_KW_IF
%token TK_KW_IN
%token TK_KW_LOCAL
%token TK_KW_NIL
%token TK_KW_WHILE
%token TK_KW_REPEAT
%token TK_KW_RETURN
%token TK_KW_THEN
%token TK_KW_UNTIL

%type <block> block
%type <statvec> scope statlist
%type <ident> identifier var /* var puede ser table pero todavia no lo consideramos */
%type <stmt> stat /*repetition*/ binding
%type <laststat> laststat
%type <idvec> namelist params setlist parlist
%type <expvec> explist1 explist23 args /* args es mas pero todavia no lo consideramos. */
%type <expr> exp function functioncal tableconstr prefixexp/* prefixexp es mas que esto pero la parte de tables todavia no lo consideramos. */
%type <condvec> condlist conds
%type <ifcond> cond
%type <tablfields> fieldlist
%type <tablfield> field

%left TK_KW_OR
%left TK_KW_AND
%left TK_OP_EQUALS TK_OP_DIFF TK_OP_MIN_EQUALS TK_OP_GRT_EQUALS TK_OP_MIN TK_OP_GRT
%right TK_OP_DOTDOT
%left TK_OP_PLUS TK_OP_MINUS
%left TK_OP_TIMES TK_OP_DIVIDED TK_OP_MOD
%right TK_KW_NOT TK_OP_HASH
%right TK_OP_EXP

%start chunk

%%

/* Creado a partir de http://lua-users.org/wiki/LuaGrammar */

chunk       : block
            {
                programBlock = $1;
            }
            ;

semi        : /* vacio */
            | TK_OP_SEMICOLON
            ;

block       : scope statlist
            {
                /*
                Creo un nuevo bloque y agrego los statements a partir
                de las listas scope y statlist (en el orden)
                */
                /* Creo un LastStatment trucho (return sin valores) */
                ExpressionList* emptyExprList = new ExpressionList();
                NLastStatement* last = new NLastStatement(0, *emptyExprList, 1); // Fake NLastStatement
                $$ = new NBlock(*last);
                $$->statements_add_list(*$1);
                $$->statements_add_list(*$2); 
            }
            | scope statlist laststat semi
            {
                /* 
                Creo un nuevo bloque y agrego los statements a partir
                de las listas scope y statlist (en el orden)
                Tambien tengo un statement final (UNICO, no como otros lenguajes)
                */
                $$ = new NBlock(*$3);
                $$->statements_add_list(*$1);
                $$->statements_add_list(*$2); 
                /* Borro scope y statlist para no dejar memoria colgada. */
                delete $1;
                delete $2;
            }
            ;

scope       : { $$ = new StatementList();}
            | scope statlist binding semi
            {
                /* cualquiera de las dos listas pueden ser vacias. */
                /* itero sobre statlist para agregar a scope. */
                
                StatementList::const_iterator it;
                for (it = (*$2).begin(); it != (*$2).end(); it++){
                    $1->push_back(*it);
                }

                $1->push_back($3);
                //for(std::vector<stmt>::iterator it = $2->begin(); it != $2->end(); ++it) {
                //    $1->push_back(it);
                //}
                /* finalmente agregando binding (definiciones local) */
                //for(std::vector<stmt>::iterator it = $3->begin(); it != $3->end(); ++it) {
                //    $1->push_back(it);
                //}
                /* Borro statlist y binding para no dejar memoria colgada. */
                //delete $2;
                //delete $3;
            }
            ;

statlist    : { $$ = new StatementList(); /* Creo una lista vacia */ } 
            | statlist stat semi
            {
                $1->push_back($2);
                /* La populo poniendo siempre al final. */
            }
            ;

identifier  : TK_ID
            {
                $$ = new NIdentifier(*$1);
                delete $1; /* TODO : Porque delete? */
            }

stat        : TK_KW_FOR identifier TK_OP_ASSIGN explist23 TK_KW_DO block TK_KW_END
            {
                $$ = new NForLoopAssign(*$2, *$4, *$6);
            }
            /* No es obligatorio
            | TK_KW_FOR namelist TK_KW_IN explist1 TK_KW_DO block TK_KW_END
            {
                $$ = new NForLoopIn(*$2, *$4, *$6);
            } */
            | TK_KW_IF conds TK_KW_END
            {
                /* TODO: Complicado */
                $$ = new NIf(*$2);
            }
            | TK_KW_FUNCTION identifier params block TK_KW_END
            {
                $$ = new NFunctionDeclaration(*$2, *$3, *$4);
            }
            | setlist TK_OP_ASSIGN explist1
            {
                $$ = new NMultiAssignment(*$1, *$3, 0);
                /* El 0 es porque NO es ```local```. */
                /* Borro las listas generadas */
                delete $1;
                delete $3;
            }
            | functioncal
            {
                /* Es un functioncall (expresion) como statement. Se usa NExpressionStatement */
                $$ = new NExpressionStatement(*$1);
            }
            ;
/*
Como solo se implementa una, se agrega la derivacion arriba.
repetition  : TK_KW_FOR identifier TK_OP_ASSIGN explist23
            {
                $$ = new NForLoopAssignNoBlock(*$2, *$<explist23>4);
            }
            | TK_KW_FOR namelist TK_KW_IN explist1
            {
                $$ = new NForLoopInNoBlock(*$<namelist>2, *<explist1>4);
            }
            ;
*/

conds       : condlist
            {
                $$ = $1;
            }
            | condlist TK_KW_ELSE block
            {
                $$ = $1;
                // El else se puede tratar como un elseif(true)
                NExpression* trueExpr = new NBoolean(1);
                NIfCond* elseCond = new NIfCond(*trueExpr, *$3);
                $$->push_back(elseCond);
            }
            ;

condlist    : cond
            {
                $$ = new ConditionList();
                $$->push_back($1);
            }
            | condlist TK_KW_ELSEIF cond
            {
                $$->push_back($3);
            }
            ;

cond        : exp TK_KW_THEN block
            {
                $$ = new NIfCond(*$1, *$3);
            }
            ;

laststat    : TK_KW_BREAK
            {
                ExpressionList* emptyExprList = new ExpressionList();
                $$ = new NLastStatement(1, *emptyExprList);
                /* se pasa por parametro si es BREAK */
            }
            | TK_KW_RETURN
            {
                ExpressionList* emptyExprList = new ExpressionList();
                $$ = new NLastStatement(0, *emptyExprList);
                /* o si no lo es. */
            }
            | TK_KW_RETURN explist1
            {
                /* Si devuelve algo entonces es impliciamente return. */ 
                $$ = new NLastStatement(0, *$2);
            }
            ;

binding     : TK_KW_LOCAL namelist
            {
                ExpressionList* emptyExprList = new ExpressionList();
                $$ = new NMultiAssignment(*$2, *emptyExprList, 1);
            }
            | TK_KW_LOCAL namelist TK_OP_ASSIGN explist1
            {
                /*
                    namelist es IdentifierList
                    explist1 es ExpressionList
                */
                $$ = new NMultiAssignment(*$2, *$4, 1);
                /* el 1 es porque es ```local``` */
                /* Borro las listas generadas */

            }
            ;

namelist    : identifier
            {
                /* inicio de lista, minimo un identifier */
                $$ = new IdentifierList();
                $$->push_back($1);
            }
            | namelist TK_OP_COMA identifier
            {
                $1->push_back($3); /* armo la lista*/
            }
            ;

explist1    : exp
            {   
                /* tiene largo minimo 3. */
                $$ = new ExpressionList();
                $$->push_back($1);
            }
            | explist1 TK_OP_COMA exp
            {
                $1->push_back($3);
                /* armo la lista de expresiones. */
            }
            ;

explist23   : exp TK_OP_COMA exp
            {
                /*
                    TODO: Ver si es necesario crear un nuevo nodo o si esta bien
                    usar una lista normal y saber que tiene largo 2 o 3.
                */
                $$ = new ExpressionList();
                $$->push_back($1);
                $$->push_back($3);
            }
            | exp TK_OP_COMA exp TK_OP_COMA exp
            {
                $$ = new ExpressionList();
                $$->push_back($1);
                $$->push_back($3);
                $$->push_back($5);
            }
            ;

exp         : TK_KW_NIL
            {
                $$ = new NNil();
            }
            | TK_STRING
            {
                std::string* final = new std::string($1->substr(1, $1->size()-2));
                $$ = new NString(*final);
            }
            | TK_BOOLEAN
            {
                /* chanchada */
                std::string trueStr ("true");
                int trueVal = 0;
                if ($1->compare(trueStr) == 0){
                    trueVal = 1;
                }
                $$ = new NBoolean(trueVal);
            }
            | TK_NUMBER_INT
            {
                $$ = new NInteger(atol($1->c_str()));
            }
            | TK_NUMBER_DOUBLE
            {
                $$ = new NDouble(atof($1->c_str()));
            }
            | TK_OP_ELIPSIS
            {
                /* No hay que implementarlas */
            }
            | function
            {
                $$ = $1;   
            }
            | prefixexp
            {
                /* TODO : Complicado */
                $$ = $1;
            }
            | tableconstr
            {
                $$ = $1;
            }
            | TK_KW_NOT exp
            {
                $$ = new NUnaryOperator(*$2, $1);
            }
            | TK_OP_HASH exp
            {
                /* No hay que usarla. */
            }
            | TK_OP_MINUS exp
            {
                $$ = new NUnaryOperator(*$2, $1);
            }
            | exp TK_KW_OR exp
            {
                $$ = new NBinaryOperator($2, *$1, *$3);
            }
            | exp TK_KW_AND exp
            {
                $$ = new NBinaryOperator($2, *$1, *$3);
            }
            | exp TK_OP_MIN exp
            {
                $$ = new NBinaryOperator($2, *$1, *$3);
            }
            | exp TK_OP_MIN_EQUALS exp
            {
                $$ = new NBinaryOperator($2, *$1, *$3);
            }
            | exp TK_OP_GRT exp
            {
                $$ = new NBinaryOperator($2, *$1, *$3);
            }
            | exp TK_OP_GRT_EQUALS exp
            {
                $$ = new NBinaryOperator($2, *$1, *$3);
            }
            | exp TK_OP_EQUALS exp
            {
                $$ = new NBinaryOperator($2, *$1, *$3);
            }
            | exp TK_OP_DIFF exp
            {
                $$ = new NBinaryOperator($2, *$1, *$3);
            }
            | exp TK_OP_DOTDOT exp
            {
                $$ = new NBinaryOperator($2, *$1, *$3);
            }
            | exp TK_OP_PLUS exp
            {
                $$ = new NBinaryOperator($2, *$1, *$3);
            }
            | exp TK_OP_MINUS exp
            {
                $$ = new NBinaryOperator($2, *$1, *$3);
            }
            | exp TK_OP_TIMES exp
            {
                $$ = new NBinaryOperator($2, *$1, *$3);
            }
            | exp TK_OP_DIVIDED exp
            {
                $$ = new NBinaryOperator($2, *$1, *$3);
            }
            | exp TK_OP_MOD exp
            {
                $$ = new NBinaryOperator($2, *$1, *$3);
            }
            | exp TK_OP_EXP exp
            {
                $$ = new NBinaryOperator($2, *$1, *$3)
            }
            ;

setlist     : var
            {
                $$ = new IdentifierList();
                $$->push_back($1);
            }
            | setlist TK_OP_COMA var
            {
                $1->push_back($3);
            }
            ;

var         : identifier
            {
                $$ = $1
            }
            | prefixexp TK_OP_OPEN_BRACK exp TK_OP_CLOS_BRACK
            {
                /* Falta manejo de tablas. */
            }
            | identifier TK_OP_DOT identifier
            {
                std::string first = $1->name;
                std::string secon = $3->name;
                first.append(".");
                first.append(secon);
                $$ = new NIdentifier(first);
            }
            ;

prefixexp   : var
            {
                /*
                    var es de tipo NIdentifier : NExpression (sin soporte a tablas por ahora)
                */
                $$ = $1
            }
            | functioncal
            {
                /* 
                    functioncall es de tipo NFunctionCall : NExpression
                */
                $$ = $1
            }
            /* Lo ignoro porque no entiendo de donde sale y puede ser el que esta dando error.
            Edit1: Probando fue el que saco el conflicto.
            */
            | TK_OP_OPEN_PAREN exp TK_OP_CLOS_PAREN
            {
                
                // exp obviamente es de tipo NExpresion, pueden ser varios.
                
                $$ = $2
            }
            ;

functioncal :
            /* ORIGINAL TODO:
            prefixexp args 
            {
                $$ = new NFunctionCall($1, $2);
                Borro la lista de argumentos. 
                delete $2;
            }
            */
            /* FIN DE ORIGINAL: TODO */
            /* SIMPLIFICACION */
            identifier TK_OP_OPEN_PAREN TK_OP_CLOS_PAREN
            {
                ExpressionList* arguments = new ExpressionList();
                $$ = new NFunctionCall(*$1, *arguments);
            }
            | identifier TK_OP_OPEN_PAREN explist1 TK_OP_CLOS_PAREN
            {
                $$ = new NFunctionCall(*$1, *$3);
            }
            | identifier TK_OP_DOT identifier TK_OP_OPEN_PAREN TK_OP_CLOS_PAREN
            {
                std::string* idenStr = new std::string($1->name + "." + $3->name); 
                NIdentifier* realid = new NIdentifier(*idenStr);
                ExpressionList* arguments = new ExpressionList();
                $$ = new NFunctionCall(*realid, *arguments);
                /* TODO: Falta deletes */
            }
            | identifier TK_OP_DOT identifier TK_OP_OPEN_PAREN explist1 TK_OP_CLOS_PAREN
            {
                std::string* idenStr = new std::string($1->name + "." + $3->name);
                NIdentifier* realid = new NIdentifier(*idenStr);
                $$ = new NFunctionCall(*realid, *$5);
                //std::cout << ((NExpression*)$$)->id.name << std::endl;
                /* TODO: Falta deletes */
                delete idenStr;
            }
            /* FIN SIMPLIFICACION */
            | prefixexp TK_OP_COLON identifier args
            {
                /* No es obligatoria. */
            }
            ;

args        : TK_OP_OPEN_PAREN TK_OP_CLOS_PAREN
            {
                $$ = new ExpressionList();
                /* lista vacia de argumentos */
            }
            | TK_OP_OPEN_PAREN explist1 TK_OP_CLOS_PAREN
            {
                /* los parentesis solo encapsulan. */
                $$ = $2
            }
            | tableconstr
            {
                /* Dificil TODO: */
            }
            | TK_STRING
            {
                $$ = new ExpressionList();
                NString* value = new NString(*$1);
                $$->push_back(value);
                delete $1;
            }
            ;

function    : TK_KW_FUNCTION params block TK_KW_END /* funcion anonima */
            {
                $$ = new NAnonFunctionDeclaration(*$2, *$3);
            }
            ; 

params      : TK_OP_OPEN_PAREN parlist TK_OP_CLOS_PAREN
            {
                $$ = $2; /* Lo unico que cambia es la encapsulacion de '(', ')'. */
            }
            ;

parlist     : /* vacio */
            {
                $$ = new IdentifierList(); /* lista vacia */
            }
            | namelist
            {
                $$ = $1; /* Lista con identifiers no vacia */
            }
            ; /* TODO: FALTAN 2 mas: "..." y "namelist ...". Ver que hacer con ... */

tableconstr : TK_OP_OPEN_BRACE TK_OP_CLOS_BRACE
            {
                GenericTableFieldList* emptyList = new GenericTableFieldList();
                NTableFieldList* fieldList = new NTableFieldList(*emptyList); 
                $$ = new NTable(*fieldList);
            }
            | TK_OP_OPEN_BRACE fieldlist TK_OP_CLOS_BRACE
            {
                $$ = new NTable(*$2);
            }
            ; /* TODO: Falta una que no entendi */

fieldlist   : field
            {
                GenericTableFieldList* fieldList = new GenericTableFieldList();
                fieldList->push_back($1);
                $$ = new NTableFieldList(*fieldList);
            }
            | fieldlist TK_OP_COMA field
            {
                $1->fieldList.push_back($3);
            }
            ; /* TODO: Falta una que no entendi. */

field       : exp
            {
                $$ = new NTableFieldSingleExpression(*$1);
            }
            | identifier TK_OP_ASSIGN exp
            {
                $$ = new NTableFieldIdentifier(*$1, *$3);
            }
            | TK_OP_OPEN_BRACK exp TK_OP_CLOS_BRACK TK_OP_ASSIGN exp
            {
                $$ = new NTableFieldExpression(*$2, *$5, false);
            }
            ;

%%

