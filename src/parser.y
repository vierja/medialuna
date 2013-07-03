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

%option debug
%option c++

%union {
    Node *node;
    NBlock *block;
    NExpression *expr;
    NStatement *stmt;
    NIdentifier *ident;
    NVariableDeclaration *var_decl;
    std::vector<NVariableDeclaration*> *varvec;
    std::vector<NExpression*> *exprvec;
    std::string *string;
    int token;
}

%token TK_ID
%token TK_NUMBER_INT
%token TK_NUMBER_DOUBLE
%token TK_STRING
%token TK_BOOLEAN
%token TK_OP_PLUS
%token TK_OP_MINUS
%token TK_OP_TIMES
%token TK_OP_DIVIDED
%token TK_OP_MOD
%token TK_OP_EXP
%token TK_OP_HASH
%token TK_OP_EQUALS
%token TK_OP_DIFF
%token TK_OP_MIN_EQUALS
%token TK_OP_GRT_EQUALS
%token TK_OP_MIN
%token TK_OP_GRT
%token TK_OP_ASSIGN
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
%token TK_OP_DOTDOT
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
%token TK_KW_NOT
%token TK_KW_AND
%token TK_KW_OR
%token TK_KW_WHILE
%token TK_KW_REPEAT
%token TK_KW_RETURN
%token TK_KW_THEN
%token TK_KW_UNTIL

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
                /* itero sobre statlist para agregar a scope. */
                for(std::vector<T>::iterator it = $<statlist>2.begin(); it != $<statlist>2.end(); ++it) {
                    $1->push_back(it);
                }
                /* finalmente agregando binding (definiciones local) */
                $1->push_back($<binding>3);
                /* Borro statlist para no dejar memoria colgada. */
                delete $2;
            }
            ;

statlist    : { $$ = new StatementList(); /* Creo una lista vacia */ } 
            | statlist stat semi { $1->push_back($<stat>2); /* La populo poniendo siempre al final. */}
            ;

identifier  : TK_ID
            {
                $$ = new NIdentifier(*$1); delete $1; 
            }

stat        : repetition TK_KW_DO block TK_KW_END
            {
                $1->block = $<block>3; /* Luego de crear repetition agrego block.*/
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
                $$ = new NMultiAssignment($1, $2);
                /* Borro las listas generadas */
                delete $1;
                delete $2;
            }
            | functioncal
            {

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

laststat    : TK_KW_BREAK { $$ = new NLastStatement(1); }
            | TK_KW_RETURN { $$ = new NLastStatement(0); }
            | TK_KW_RETURN explist1 { $$ = new NLastStatement($<explist1>1); }
            ;

binding     : TK_KW_LOCAL namelist {$$ = }
            | TK_KW_LOCAL namelist TK_OP_ASSIGN explist1
            ;

namelist    : identifier
            {
                $$ = new IdentifierList(); $$->push_back($1);
            }
            | namelist TK_OP_COMA identifier
            {
                $1->push_back(*3); /* armo la lista*/
            }
            ;

explist1    : exp
            | explist1 TK_OP_COMA exp
            ;

explist23   : exp TK_OP_COMA exp
            | exp TK_OP_COMA exp TK_OP_COMA exp
            ;

exp         : TK_KW_NIL
            | TK_BOOLEAN
            | TK_NUMBER_INT
            | TK_NUMBER_DOUBLE
            | TK_OP_ELIPSIS
            | function
            | prefixexp
            | tableconstr
            | TK_KW_NOT exp
            | TK_OP_HASH exp
            | TK_OP_MINUS exp
            | exp TK_KW_AND exp
            | exp TK_KW_OR exp
            | exp TK_OP_MIN exp
            | exp TK_OP_MIN_EQUALS exp
            | exp TK_OP_GRT exp
            | exp TK_OP_GRT_EQUALS exp
            | exp TK_OP_EQUALS exp
            | exp TK_OP_DIFF exp
            | exp TK_OP_DOTDOT exp
            | exp TK_OP_PLUS exp
            | exp TK_OP_MINUS exp
            | exp TK_OP_TIMES exp
            | exp TK_OP_DIVIDED exp
            | exp TK_OP_MOD exp
            | exp TK_OP_EXP exp
            ;

setlist     : var
            | setlist TK_OP_COMA var
            ;

var         : identifier
            | prefixexp TK_OP_OPEN_BRACK exp TK_OP_CLOS_BRACK
            | prefixexp TK_OP_DOT identifier
            ;

prefixexp   : var
            | functioncal
            | TK_OP_OPEN_PAREN exp TK_OP_CLOS_PAREN
            ;

functioncal : prefixexp args
            {
                $$ = new NFunctionCall($1, $2);
                /* Borro la lista de argumentos. */
                delete $2;
            }
            | prefixexp TK_OP_COLON identifier args
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
                $$ = $1; /* Lo unico que cambia es la encapsulacion de '(', ')'. */
            }
            ;

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

