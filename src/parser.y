%{
#include <string>
#include "scanner.h"
#define YYSTYPE Token // tipo de yylval
%}

%start program

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

%%

program     : /* vacio - un programa vacio es valido */
            | block
            ;

block       : /* vacio */
            | stat opt_colon block
            | laststat opt_colon
            ;

stat        : varlist TK_OP_ASSIGN explist
            | funcall
            | TK_KW_DO block TK_KW_END
            | TK_KW_WHILE exp TK_KW_UNTIL block TK_KW_END
            | TK_KW_REPEAT block TK_KW_UNTIL exp
            | TK_KW_IF exp TK_KW_THEN block elseif_list opt_else TK_KW_END
            | TK_KW_FOR TK_ID TK_OP_ASSIGN exp TK_OP_COMA exp opt_com_exp TK_KW_DO block TK_KW_END
            | TK_KW_FUNCTION funcname funcbody
            | TK_KW_LOCAL TK_KW_FUNCTION TK_ID funcbody
            | TK_KW_LOCAL namelist opt_explist
            ;

elseif_list : /* vacio */
            | TK_KW_ELSEIF exp TK_KW_THEN block elseif_list
            ;

opt_else    : /* vacio */
            | TK_KW_ELSE block
            ;

laststat    : TK_KW_RETURN
            | TK_KW_RETURN explist
            | TK_KW_BREAK
            ;

funcname    : TK_ID list_name op_colon_na;

list_name   : /* vacio */
            | TK_OP_DOT TK_ID
            ;

op_colon_na : /* vacio */
            | TK_OP_COLON
            ;

varlist     : var list_var;

list_var    : /* vacio */
            | TK_OP_COMA var
            ;

var         : TK_ID
            | prefixexp TK_OP_OPEN_BRACK exp TK_OP_CLOS_BRACK
            | prefixexp TK_OP_DOT TK_ID
            ;

namelist    : TK_ID l_namelist;

explist     : exp_comm_li exp;

exp_comm_li : /* vacio */
            | TK_OP_COMA exp exp_comm_li
            ;

exp         : TK_KW_NIL
            | TK_BOOLEAN
            | TK_NUMBER_INT
            | TK_NUMBER_DOUBLE
            | TK_ID
            | TK_OP_ELIPSIS
            | function
            | prefixexp
            | tableconstr
            | exp binop exp
            | unop exp
            ;

prefixexp   : var
            | funcall
            | TK_OP_OPEN_PAREN exp TK_OP_CLOS_PAREN
            ;

funcall     : prefixexp args
            | prefixexp TK_OP_COLON TK_ID args
            ;

args        : TK_OP_OPEN_PAREN TK_OP_CLOS_PAREN block TK_KW_END
            | TK_OP_OPEN_PAREN explist TK_OP_CLOS_PAREN block TK_KW_END
            ;

function    : TK_KW_FUNCTION funcbody;

funcbody    : TK_OP_OPEN_PAREN TK_OP_CLOS_PAREN block TK_KW_END
            | TK_OP_OPEN_PAREN parlist TK_OP_CLOS_PAREN block TK_KW_END
            ;

parlist     : namelist
            | namelist TK_OP_DOT TK_OP_ELIPSIS
            | TK_OP_ELIPSIS
            ;

tableconstr : TK_OP_OPEN_BRACE TK_OP_CLOS_BRACE
            | TK_OP_OPEN_BRACE fieldlist TK_OP_CLOS_BRACE
            ;

fieldlist   : field fieldlist_o
            | field fieldlist_o fieldsep
            ;

fieldlist_o :
            | fieldsep field fieldlist_o
            ;

field       : TK_OP_OPEN_BRACK exp TK_OP_CLOS_BRACK TK_OP_ASSIGN exp
            | TK_ID TK_OP_ASSIGN exp
            | exp
            ;

fieldsep    : TK_OP_COMA
            | TK_OP_SEMICOLON
            ;

binop       : TK_OP_PLUS
            | TK_OP_MINUS
            | TK_OP_TIMES
            | TK_OP_DIVIDED
            | TK_OP_MOD
            | TK_OP_EXP
            | TK_OP_EQUALS
            | TK_OP_DIFF
            | TK_OP_MIN_EQUALS
            | TK_OP_GRT_EQUALS
            | TK_OP_MIN
            | TK_OP_GRT
            | TK_KW_AND
            | TK_KW_OR
            ;

unop        : TK_OP_MINUS
            | TK_KW_NOT
            | TK_OP_HASH
            ;

l_namelist  : /* vacio */
            | TK_OP_COMA TK_ID
            ;

opt_colon   : /* vacio */
            | TK_OP_SEMICOLON
            ;

opt_explist : /* vacio */
            | TK_OP_ASSIGN explist
            ;

opt_com_exp : /* vacio */
            | TK_OP_COMA exp
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
yyerror(char *s){
    printf("%s\n", s);
}

