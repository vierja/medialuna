%{
#include <string>
#include "scanner.h"
#define YYSTYPE Token // tipo de yylval
%}

%start chunk

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

/* Creado a partir de http://lua-users.org/wiki/LuaGrammar */

chunk       : block ;

semi        :
            | TK_OP_SEMICOLON
            ;

block       : scope statlist
            | scope statlist laststat semi
            ;

ublock      : block TK_KW_UNTIL exp ;

scope       :
            | scope statlist binding semi
            ;

statlist    :
            | statlist stat semi
            ;

stat        : TK_KW_DO block TK_KW_END
            | TK_KW_WHILE exp TK_KW_UNTIL block TK_KW_END
            | repetition TK_KW_DO block TK_KW_END
            | TK_KW_REPEAT ublock
            | TK_KW_IF conds TK_KW_END
            | TK_KW_FUNCTION funcname funcbody
            | setlist TK_OP_ASSIGN explist1
            | functioncal
            ;

repetition  : TK_KW_FOR TK_ID TK_OP_ASSIGN explist23
            | TK_KW_FOR namelist TK_KW_IN explist1
            ;

conds       : condlist
            | condlist TK_KW_ELSE block
            ;

condlist    : cond
            | condlist TK_KW_ELSEIF cond
            ;

cond        : exp TK_KW_THEN block ;

laststat    : TK_KW_BREAK
            | TK_KW_RETURN
            | TK_KW_RETURN explist1
            ;

binding     : TK_KW_LOCAL namelist
            | TK_KW_LOCAL namelist TK_OP_ASSIGN explist1
            | TK_KW_LOCAL TK_KW_FUNCTION TK_ID funcbody
            ;

funcname    : dottedname
            | dottedname TK_OP_COLON TK_ID
            ;

dottedname  : TK_ID
            | dottedname TK_OP_DOT TK_ID
            ;

namelist    : TK_ID
            | namelist TK_OP_COMA TK_ID
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

var         : TK_ID
            | prefixexp TK_OP_OPEN_BRACK exp TK_OP_CLOS_BRACK
            | prefixexp TK_OP_DOT TK_ID
            ;

prefixexp   : var
            | functioncal
            | TK_OP_OPEN_PAREN exp TK_OP_CLOS_PAREN
            ;

functioncal : prefixexp args
            | prefixexp TK_OP_COLON TK_ID args
            ;

args        : TK_OP_OPEN_PAREN TK_OP_CLOS_PAREN
            | TK_OP_OPEN_PAREN explist1 TK_OP_CLOS_PAREN
            | tableconstr
            | TK_STRING
            ;

function    : TK_KW_FUNCTION funcbody ;

funcbody    : params block TK_KW_END ;

params      : TK_OP_OPEN_PAREN parlist TK_OP_CLOS_PAREN ;

parlist     :
            | namelist
            | TK_OP_ELIPSIS
            | namelist TK_OP_COMA TK_OP_ELIPSIS
            ;

tableconstr : TK_OP_OPEN_BRACE TK_OP_CLOS_BRACE
            | TK_OP_OPEN_BRACE fieldlist TK_OP_CLOS_BRACE
            ; /* TODO: Falta una que no entendi */

fieldlist   : field
            | fieldlist TK_OP_COMA field
            ; /* TODO: Falta una que no entendi. */

field       : exp
            | TK_ID TK_OP_ASSIGN exp
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
yyerror(char *s){
    printf("%s\n", s);
}

