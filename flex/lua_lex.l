DIGIT       [0-9]
HEX         [0-9a-fA-F]
ID          [a-zA-Z_][a-zA-Z0-9_]*
RESERVED    and|break|do|else|elseif|end|for|function|if|in|local|nil|not|or|repeat|return|then|until
BOOL        true|false
OPERATOR    "+"|"-"|"*"|"/"|"%"|"^"|"#"|"=="|"~="|"<="|">="|"<"|">"|"="|"("|")"|"{"|"}"|"["|"]"|";"|":"|","|"."|".."|"..."
NIL         nil

%%

{DIGIT}+                                            { printf("int: %s (%d)\n", yytext, atoi(yytext)); }

{DIGIT}+"."{DIGIT}*                                 { printf("float: %s (%g)\n", yytext, atof(yytext)); }

{DIGIT}+"."?{DIGIT}*("e"|"E")("+"|"-")?{DIGIT}+     { printf("scientific notation number: %s\n", yytext); }

0x{HEX}+                                            { printf("hex: %s\n", yytext); }

{BOOL}                      { printf("boolean: %s\n", yytext); }

{NIL}                       { printf("null: %s\n", yytext); }

{OPERATOR}                  { printf("operator: %s\n", yytext); }

{RESERVED}                  { printf("reserved keyword: %s\n", yytext); }

\'[^\'\n]*\'|\"[^\"\n]*\"   { printf("strings, value: %s\n", yytext); }

"[["\n[^(\]\])]*"]]"        { printf("multiple line string 1, value: %s\n", yytext); }

"[["[^(\]\])]*"]]"          { printf("multiple line string 2, value: %s\n", yytext); }

"[==["\n[^(\]==\])]*"]==]"  { printf("multiple line string 3, value: %s\n", yytext); }

"[==["[^(\]\])]*"]==]"      { printf("multiple line string 4, value: %s\n", yytext); }

"--[["[^(\]\])]*"]]"        { printf("comentario de multiples lineas.\n"); }

"--".*$                     { printf("comentario de una linea.\n"); }

{ID}                        { printf("Id word: %s\n", yytext); }

[ \t\n]+                    { /* Ignora espacios */ }

.                           { printf("Error, value: %s\n", yytext); }

%%

int main( int argc, char **argv )
{
    ++argv, --argc;
    if ( argc > 0 )
        yyin = fopen( argv[0], "r" );
    else
        yyin = stdin;

    yylex();
}


