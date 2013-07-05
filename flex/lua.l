DIGIT       [0-9]
HEX         [0-9a-fA-F]
ID          [a-z_][a-z0-9_]*
BOOL        true|false

%%

{DIGIT}+                                            { printf("int: %s (%d)\n", yytext, atoi(yytext)); return(VALUE); }

{DIGIT}+"."{DIGIT}*                                 { printf("float: %s (%g)\n", yytext, atof(yytext)); return(VALUE); }

{DIGIT}+"."?{DIGIT}*("e"|"E")("+"|"-")?{DIGIT}+     { printf("scientific notation number: %s\n", yytext); return(VALUE); }

0x{HEX}+                                            { printf("hex: %s\n", yytext); return(VALUE); }

{BOOL}                      { printf("boolean: %s\n", yytext); return(BOOLEAN); }

"+"                         { printf("+\n");        return(PLUS);       }
"-"                         { printf("-\n");        return(MINUS);      }
"*"                         { printf("*\n");        return(TIMES);      }
"/"                         { printf("/\n");        return(DIVIDED);    }
"%"                         { printf("%\n");        return(MOD);        }
"^"                         { printf("^\n");        return(EXP);        }
"#"                         { printf("#\n");        return(HASH);       }
"=="                        { printf("==\n");       return(EQUALS);     }
"~="                        { printf("~=\n");       return(DIFF);       }
"<="                        { printf("<=\n");       return(MIN_EQUALS); }
">="                        { printf(">=\n");       return(GRT_EQUALS); }
"<"                         { printf("<\n");        return(MIN);        }
">"                         { printf(">\n");        return(GRT);        }
"="                         { printf("=\n");        return(ASSIGN);     }
"("                         { printf("(\n");        return(OPEN_PAREN); }
")"                         { printf(")\n");        return(CLOS_PAREN); }
"{"                         { printf("{\n");        return(OPEN_BRACE); }
"}"                         { printf("}\n");        return(CLOS_BRACE); }
"["                         { printf("[\n");        return(OPEN_BRACK); }
"]"                         { printf("]\n");        return(CLOS_BRACK); }
";"                         { printf(";\n");        return(SEMICOLON);  }
":"                         { printf(":\n");        return(COLON);      }
","                         { printf(",\n");        return(COMA);       }
"."                         { printf(".\n");        return(DOT);        }
".."                        { printf("..\n");       return(DOTDOT);     }
"..."                       { printf("...\n");      return(ELIPSIS);    }

and                         { printf("and");        return(AND);        }
break                       { printf("break");      return(BREAK);      }
do                          { printf("do");         return(DO);         }
else                        { printf("else");       return(ELSE);       }
elseif                      { printf("elseif");     return(ELSEIF);     }
end                         { printf("end");        return(END);        }
for                         { printf("for");        return(FOR);        }
function                    { printf("function");   return(FUNCTION);   }
if                          { printf("if");         return(IF);         }
in                          { printf("in");         return(IN);         }
local                       { printf("local");      return(LOCAL);      }
nil                         { printf("nil");        return(NIL);        }
not                         { printf("not");        return(NOT);        }
or                          { printf("or");         return(OR);         }
repeat                      { printf("repeat");     return(REPEAT);     }
return                      { printf("return");     return(RETURN);     }
then                        { printf("then");       return(THEN);       }
until                       { printf("until");      return(UNTIL);      }

\'[^\"\n]*\'|\"[^\']*\"     { printf("string, value: %s\n", yytext); }

"[["\n[^(\]\])]*"]]"        { printf("multiple line string, value: %s\n", yytext); }

"[["[^(\]\])]*"]]"          { printf("multiple line string, value: %s\n", yytext); }

"[==["\n[^(\]==\])]*"]==]"  { printf("multiple line string, value: %s\n", yytext); }

"[==["[^(\]\])]*"]==]"      { printf("multiple line string, value: %s\n", yytext); }

"--".*$                     { printf("comentario de una linea.\n"); }

{ID}                        { printf("Id word: %s\n", yytext); return(ID); }

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


