DIGIT       [0-9]
HEX         [0-9a-fA-F]
ID          [a-z_][a-z0-9_]*
RESERVED    and|break|do|else|elseif|end|for|function|if|in|local|nil|not|or|repeat|return|then|until
BOOL        true|false
OPERATOR    "+"|"-"|"*"|"/"|"%"|"^"|"#"|"=="|"~="|"<="|">="|"<"|">"|"="|"("|")"|"{"|"}"|"["|"]"|";"|":"|","|"."|".."|"..."
NIL         nil

%%

{DIGIT}+                                            { printf("int: %s (%d)\n", yytext, atoi(yytext)); }

{DIGIT}+"."{DIGIT}*                                 { printf("float: %s (%g)\n", yytext, atof(yytext)); }


{DIGIT}+"."?{DIGIT}*("e"|"E")("+"|"-")?{DIGIT}+     { printf("scientific notation number: %s\n", yytext); }

0x{HEX}+                                            { printf("hex: %s\n", yytext); }


{BOOL}                  { printf("boolean: %s\n", yytext); }

{NIL}                   { printf("null: %s\n", yytext); }

{OPERATOR}              { printf("operator: %s\n", yytext); }

{RESERVED}              { printf("reserved keyword: %s\n", yytext); }


\'[^\"\n]*\'|\"[^\']*\"     { printf("string, value: %s\n", yytext); }

"[["\n[^(\]\])]*"]]"        { printf("multiple line string, value: %s\n", yytext); }

"[["[^(\]\])]*"]]"          { printf("multiple line string, value: %s\n", yytext); }

"[==["\n[^(\]==\])]*"]==]"  { printf("multiple line string, value: %s\n", yytext); }

"[==["[^(\]\])]*"]==]"      { printf("multiple line string, value: %s\n", yytext); }

"--".*$                     { printf("comentario de una linea.\n"); }

{ID}                        { printf("Id word: %s\n", yytext); }

[ \t\n]+                    { /* Ignora espacios */ }

.                           { printf("Error, value: %s\n", yytext); }

%%

int main( int argc, char **argv )
{
    ++argv, --argc;  /* skip over program name */
    if ( argc > 0 )
        yyin = fopen( argv[0], "r" );
    else
        yyin = stdin;

    yylex();
}


