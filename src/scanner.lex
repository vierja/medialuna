%{
#include <string>
#include "node.hpp"
#include "parser.hpp"
#define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
extern "C" int yywrap() { }
%}

SPACES              [ \t\n\v\f\r]+
DIGIT               [0-9]
HEX                 [0-9a-fA-F]
BOOL                true|false
ID                  [A-Za-z_][A-Za-z0-9_]*
STRING_SMPL         \'[^\'\n]*\'
STRING_DBL          \"[^\"\n]*\"
MULTI_LINE_STR_1    "[["\n[^(\]\])]*"]]"
MULTI_LINE_STR_2    "[["[^(\]\])]*"]]"
MULTI_LINE_STR_3    "[==["\n[^(\]==\])]*"]==]"
MULTI_LINE_STR_4    "[==["[^(\]\])]*"]==]"
SCIENTIFIC          {DIGIT}+"."?{DIGIT}*("e"|"E")("+"|"-")?{DIGIT}+

%%

"--".*$             ;// ignoro comentarios.

{SPACES}            ;// ignoro espacios,tabs, etc.

{DIGIT}+            SAVE_TOKEN; return TK_NUMBER_INT; 
{DIGIT}+"."{DIGIT}* SAVE_TOKEN; return TK_NUMBER_DOUBLE; 
{SCIENTIFIC}        SAVE_TOKEN; return TK_NUMBER_DOUBLE;  // notacion cientifica
0[x|X]{HEX}         SAVE_TOKEN; return TK_NUMBER_INT;  // notacion HEX
{BOOL}              SAVE_TOKEN; return TK_BOOLEAN; 

{STRING_SMPL}       SAVE_TOKEN; return TK_STRING;
{STRING_DBL}        SAVE_TOKEN; return TK_STRING;
{MULTI_LINE_STR_1}  SAVE_TOKEN; return TK_STRING;
{MULTI_LINE_STR_2}  SAVE_TOKEN; return TK_STRING;
{MULTI_LINE_STR_3}  SAVE_TOKEN; return TK_STRING;
{MULTI_LINE_STR_4}  SAVE_TOKEN; return TK_STRING;        


"+"                 return TOKEN(TK_OP_PLUS);
"-"                 return TOKEN(TK_OP_MINUS);
"*"                 return TOKEN(TK_OP_TIMES);
"/"                 return TOKEN(TK_OP_DIVIDED);
"%"                 return TOKEN(TK_OP_MOD);
"^"                 return TOKEN(TK_OP_EXP);
"#"                 return TOKEN(TK_OP_HASH);
"=="                return TOKEN(TK_OP_EQUALS);
"~="                return TOKEN(TK_OP_DIFF);
"<="                return TOKEN(TK_OP_MIN_EQUALS);
">="                return TOKEN(TK_OP_GRT_EQUALS);
"<"                 return TOKEN(TK_OP_MIN);
">"                 return TOKEN(TK_OP_GRT);
"="                 return TOKEN(TK_OP_ASSIGN);
"("                 return TOKEN(TK_OP_OPEN_PAREN);
")"                 return TOKEN(TK_OP_CLOS_PAREN);
"{"                 return TOKEN(TK_OP_OPEN_BRACE);
"}"                 return TOKEN(TK_OP_CLOS_BRACE);
"["                 return TOKEN(TK_OP_OPEN_BRACK);
"]"                 return TOKEN(TK_OP_CLOS_BRACK);
";"                 return TOKEN(TK_OP_SEMICOLON);
":"                 return TOKEN(TK_OP_COLON);
","                 return TOKEN(TK_OP_COMA);
"."                 return TOKEN(TK_OP_DOT);
".."                return TOKEN(TK_OP_DOTDOT);
"..."               return TOKEN(TK_OP_ELIPSIS);

and                 return TOKEN(TK_KW_AND);
break               return TOKEN(TK_KW_BREAK);
do                  return TOKEN(TK_KW_DO);
else                return TOKEN(TK_KW_ELSE);
elseif              return TOKEN(TK_KW_ELSEIF);
end                 return TOKEN(TK_KW_END);
for                 return TOKEN(TK_KW_FOR);
function            return TOKEN(TK_KW_FUNCTION);
if                  return TOKEN(TK_KW_IF);
in                  return TOKEN(TK_KW_IN);
local               return TOKEN(TK_KW_LOCAL);
nil                 return TOKEN(TK_KW_NIL);
not                 return TOKEN(TK_KW_NOT);
or                  return TOKEN(TK_KW_OR);
while               return TOKEN(TK_KW_WHILE);
repeat              return TOKEN(TK_KW_REPEAT);
return              return TOKEN(TK_KW_RETURN);
then                return TOKEN(TK_KW_THEN);
until               return TOKEN(TK_KW_UNTIL);

{ID}                SAVE_TOKEN; return TK_ID;

.                   { printf("Unkown token! value: %s\n", yytext); yyterminate(); /* error */ }

%%