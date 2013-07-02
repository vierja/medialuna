%option c++
%option yylineno

%{
#include "scanner.h"
%}

SPACES              [ \t\n\v\f\r]+
DIGIT               [0-9]
HEX                 [0-9a-fA-F]
BOOL                true|false
ID                  [a-z_][a-z0-9_]*
STRING              \'[^\"\n]*\'|\"[^\']*\"
MULTI_LINE_STR_1    "[["\n[^(\]\])]*"]]"
MULTI_LINE_STR_2    "[["[^(\]\])]*"]]"
MULTI_LINE_STR_3    "[==["\n[^(\]==\])]*"]==]"
MULTI_LINE_STR_4    "[==["[^(\]\])]*"]==]"
SCIENTIFIC          {DIGIT}+"."?{DIGIT}*("e"|"E")("+"|"-")?{DIGIT}+


%%

{SPACES}            // ignoro espacios,tabs, etc.

{DIGIT}+            return TK_NUMBER_INT; 
{DIGIT}+"."{DIGIT}* return TK_NUMBER_DOUBLE; 
{SCIENTIFIC}        return TK_NUMBER_DOUBLE;  // notacion cientifica
0[x|X]{HEX}         return TK_NUMBER_INT;  // notacion HEX
{BOOL}              return TK_BOOLEAN; 

{STRING}            return TK_STRING; 
{MULTI_LINE_STR_1}  return TK_STRING; 
{MULTI_LINE_STR_2}  return TK_STRING; 
{MULTI_LINE_STR_3}  return TK_STRING; 
{MULTI_LINE_STR_4}  return TK_STRING; 

"+"                 return TK_OP_PLUS; 
"-"                 return TK_OP_MINUS; 
"*"                 return TK_OP_TIMES; 
"/"                 return TK_OP_DIVIDED; 
"%"                 return TK_OP_MOD; 
"^"                 return TK_OP_EXP; 
"#"                 return TK_OP_HASH; 
"=="                return TK_OP_EQUALS; 
"~="                return TK_OP_DIFF; 
"<="                return TK_OP_MIN_EQUALS; 
">="                return TK_OP_GRT_EQUALS; 
"<"                 return TK_OP_MIN; 
">"                 return TK_OP_GRT; 
"="                 return TK_OP_ASSIGN; 
"("                 return TK_OP_OPEN_PAREN; 
")"                 return TK_OP_CLOS_PAREN; 
"{"                 return TK_OP_OPEN_BRACE; 
"}"                 return TK_OP_CLOS_BRACE; 
"["                 return TK_OP_OPEN_BRACK; 
"]"                 return TK_OP_CLOS_BRACK; 
";"                 return TK_OP_SEMICOLON; 
":"                 return TK_OP_COLON; 
","                 return TK_OP_COMA; 
"."                 return TK_OP_DOT; 
".."                return TK_OP_DOTDOT; 
"..."               return TK_OP_ELIPSIS; 

and                 return TK_KW_AND; 
break               return TK_KW_BREAK; 
do                  return TK_KW_DO; 
else                return TK_KW_ELSE; 
elseif              return TK_KW_ELSEIF; 
end                 return TK_KW_END; 
for                 return TK_KW_FOR; 
function            return TK_KW_FUNCTION; 
if                  return TK_KW_IF; 
in                  return TK_KW_IN; 
local               return TK_KW_LOCAL; 
nil                 return TK_KW_NIL; 
not                 return TK_KW_NOT; 
or                  return TK_KW_OR; 
while               return TK_KW_WHILE; 
repeat              return TK_KW_REPEAT; 
return              return TK_KW_RETURN; 
then                return TK_KW_THEN; 
until               return TK_KW_UNTIL; 

{ID}                return TK_ID; 

.                   /* error */

%%