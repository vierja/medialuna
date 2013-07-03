%{
    #include <string>
    #include "scanner.h"
    #include "node.h"

    NBlock *programBlock;

    /*#define YYSTYPE Token // tipo de yylval*/
    extern int yylex();
    void yyerror(const char *);
    #define YYDEBUG 1 /* For Debugging */
    int errors;
%}


%union {
    Node *node;
    NBlock *block;
    NExpression *expr;
    NStatement *stmt;
    NIdentifier *ident;
    NLastStatement *laststat;
    NVariableDeclaration *var_decl;
    std::vector<NVariableDeclaration*> *varvec;
    std::vector<NExpression*> *exprvec;
    std::vector<NStatement*> *statvec;
    std::vector<NIdentifier*> *idvec;
    std::vector<NExpression*> *expvec;
    std::string *string;
    int token;
}

%token <string> TK_ID TK_NUMBER_INT TK_NUMBER_DOUBLE TK_STRING TK_BOOLEAN
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
%type <stmt> stat repetition functioncal binding
%type <laststat> laststat
%type <idvec> namelist params setlist
%type <expvec> explist1 explist23 args /* args es mas pero todavia no lo consideramos. */
%type <expr> exp function prefixexp /* prefixexp es mas que esto pero la parte de tables todavia no lo consideramos. */

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

chunk       : block { programBlock = $1; }
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
                $$ = new NBlock();
                $$->statements_add_list($<scope>1);
                $$->statements_add_list($<statlist>2); 
            }
            | scope statlist laststat semi
            {
                /* 
                Creo un nuevo bloque y agrego los statements a partir
                de las listas scope y statlist (en el orden)
                Tambien tengo un statement final (UNICO, no como otros lenguajes)
                */
                $$ = new NBlock($<laststat>3);
                $$->statements_add_list($<scope>1);
                $$->statements_add_list($<statlist>2); 
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
                for(std::vector<T>::iterator it = $<statlist>2.begin(); it != $<statlist>2.end(); ++it) {
                    $1->push_back(it);
                }
                /* finalmente agregando binding (definiciones local) */
                for(std::vector<T>::iterator it = $<binding>3.begin(); it != $<binding>3.end(); ++it) {
                    $1->push_back(it);
                }
                /* Borro statlist y binding para no dejar memoria colgada. */
                delete $2;
                delete $3;
            }
            ;

statlist    : { $$ = new StatementList(); /* Creo una lista vacia */ } 
            | statlist stat semi
            {
                $1->push_back($<stat>2);
                /* La populo poniendo siempre al final. */
            }
            ;

identifier  : TK_ID
            {
                $$ = new NIdentifier(*$1);
                delete $1; /* TODO : Porque delete? */
            }

stat        : repetition TK_KW_DO block TK_KW_END
            {
                $1->block = $<block>3;
                /* Luego de crear repetition agrego block. */
            }
            | TK_KW_IF conds TK_KW_END
            {
                /* TODO: Complicado */
            }
            | TK_KW_FUNCTION identifier params block TK_KW_END
            {
                $$ = new NFunctionDeclaration(*$2, *$3, *$4);
            }
            | setlist TK_OP_ASSIGN explist1
            {
                $$ = new NMultiAssignment($1, $2, 0);
                /* El 0 es porque NO es ```local```. */
                /* Borro las listas generadas */
                delete $1;
                delete $2;
            }
            | functioncal
            {
                /* Se crea en su regla */
                $$ = $1;
            }
            ;

repetition  : TK_KW_FOR identifier TK_OP_ASSIGN explist23
            {
                $$ = new NForLoopAssign(*$2, *$<explist23>4);
            }
            | TK_KW_FOR namelist TK_KW_IN explist1
            {
                $$ = new NForLoopIn(*$<namelist>2, *<explist1>4);
            }
            ;

conds       : condlist
            | condlist TK_KW_ELSE block
            ;

condlist    : cond
            | condlist TK_KW_ELSEIF cond
            ;

cond        : exp TK_KW_THEN block ;

laststat    : TK_KW_BREAK
            {
                $$ = new NLastStatement(1);
                /* se pasa por parametro si es BREAK */
            }
            | TK_KW_RETURN
            {
                $$ = new NLastStatement(0);
                /* o si no lo es. */
            }
            | TK_KW_RETURN explist1
            {
                /* Si devuelve algo entonces es impliciamente return. */ 
                $$ = new NLastStatement($<explist1>1);
            }
            ;

binding     : TK_KW_LOCAL namelist
            {
                $$ = new NMultiVariableDeclaration(1);
                $$->isList = $2;
                delete $2;
            }
            | TK_KW_LOCAL namelist TK_OP_ASSIGN explist1
            {
                /*
                    namelist es IdentifierList
                    explist1 es ExpressionList
                */
                $$ = new NMultiAssignment($2, $4, 1);
                /* el 1 es porque es ```local``` */
                /* Borro las listas generadas */
                delete $2;
                delete $4;
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
            | TK_BOOLEAN
            {
                /* chanchada */
                $$ = new NBoolean($1 == "true");
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
            }
            | tableconstr
            {
                /* TODO : No me quiero meter con tablas por ahora. */
            }
            | TK_KW_NOT exp
            {
                $$ = new NUnaryOperator($2, $1);
            }
            | TK_OP_HASH exp
            {
                /* No hay que usarla. */
            }
            | TK_OP_MINUS exp
            {
                $$ = new NUnaryOperator($2, $1);
            }
            | exp TK_KW_OR exp
            {
                $$ = NBinaryOperator($2, $1, $3);
            }
            | exp TK_KW_AND exp
            {
                $$ = NBinaryOperator($2, $1, $3);
            }
            | exp TK_OP_MIN exp
            {
                $$ = NBinaryOperator($2, $1, $3);
            }
            | exp TK_OP_MIN_EQUALS exp
            {
                $$ = NBinaryOperator($2, $1, $3);
            }
            | exp TK_OP_GRT exp
            {
                $$ = NBinaryOperator($2, $1, $3);
            }
            | exp TK_OP_GRT_EQUALS exp
            {
                $$ = NBinaryOperator($2, $1, $3);
            }
            | exp TK_OP_EQUALS exp
            {
                $$ = NBinaryOperator($2, $1, $3);
            }
            | exp TK_OP_DIFF exp
            {
                $$ = NBinaryOperator($2, $1, $3);
            }
            | exp TK_OP_DOTDOT exp
            {
                $$ = NBinaryOperator($2, $1, $3);
            }
            | exp TK_OP_PLUS exp
            {
                $$ = NBinaryOperator($2, $1, $3);
            }
            | exp TK_OP_MINUS exp
            {
                $$ = NBinaryOperator($2, $1, $3);
            }
            | exp TK_OP_TIMES exp
            {
                $$ = NBinaryOperator($2, $1, $3);
            }
            | exp TK_OP_DIVIDED exp
            {
                $$ = NBinaryOperator($2, $1, $3);
            }
            | exp TK_OP_MOD exp
            {
                $$ = NBinaryOperator($2, $1, $3);
            }
            | exp TK_OP_EXP exp
            {
                $$ = NBinaryOperator($2, $1, $3)
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
            | prefixexp TK_OP_DOT identifier
            {
                /* Creo que no se debe implementar */
                /* TODO: CHEQUEAR */
            }
            ;

prefixexp   : var
            {
                /*
                    var es de tipo NIdentifier : NExpression (sin soporte a tablas)
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
            | TK_OP_OPEN_PAREN exp TK_OP_CLOS_PAREN
            {
                /*
                    exp obviamente es de tipo NExpresion, pueden ser varios.
                */
                $$ = $2
            }
            ;

functioncal : prefixexp args
            {
                $$ = new NFunctionCall($1, $2);
                /* Borro la lista de argumentos. */
                delete $2;
            }
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
                NString value = new NString($1);
                $$->push_back(value);
            }
            ;

function    : TK_KW_FUNCTION params block TK_KW_END /* funcion anonima */
            {
                $$ = new NAnonFunctionDeclaration(*$2, *$3);
            }
            ; 

params      : TK_OP_OPEN_PAREN namelist TK_OP_CLOS_PAREN
            {
                $$ = $2; /* Lo unico que cambia es la encapsulacion de '(', ')'. */
            }
            ;

/* Ignoramos las tables hasta tener una version funcionando */
tableconstr : TK_OP_OPEN_BRACE TK_OP_CLOS_BRACE
            | TK_OP_OPEN_BRACE fieldlist TK_OP_CLOS_BRACE
            ; /* TODO: Falta una que no entendi */

fieldlist   : field
            | fieldlist TK_OP_COMA field
            ; /* TODO: Falta una que no entendi. */

field       : exp
            | identifier TK_OP_ASSIGN exp
            | TK_OP_OPEN_BRACK exp TK_OP_CLOS_BRACK TK_OP_ASSIGN exp
            ;

%%

/* para probar */
main(int argc, char* argv[]){
    extern FILE *yyin;
    ++argv, --argc;
    yyin = fopen(argv[0], "r");
    yydebug = 1;
    errors = 0;
    yyparse();
}
void yyerror(const char *s){
    printf("%s\n", s);
}

