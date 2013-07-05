
%union semrec
{
    int intval;
    char* id;
    struct 
}

%start program
%token 
        VALUE
        BOOLEAN
        PLUS
        MINUS
        TIMES
        DIVIDED
        MOD
        EXP
        HASH
        EQUALS
        DIFF
        MIN_EQUALS
        GRT_EQUALS
        MIN
        GRT
        ASSIGN
        OPEN_PAREN
        CLOS_PAREN
        OPEN_BRACE
        CLOS_BRACE
        OPEN_BRACK
        CLOS_BRACK
        SEMICOLON
        COLON
        COMA
        DOT
        DOTDOT
        ELIPSIS
        AND
        BREAK
        DO
        ELSE
        ELSEIF
        END
        FOR
        FUNCTION
        IF
        IN
        LOCAL
        NIL
        NOT
        OR
        REPEAT
        RETURN
        THEN
        UNTIL
        ID

%left


chunk ::= {stat [`;´]} [laststat [`;´]]

block ::= chunk

stat ::=  varlist `=´ explist | 
     functioncall | 
     DO block END | 
     WHILE exp DO block END | 
     REPEAT block UNTIL exp | 
     IF exp THEN block {ELSEIF exp THEN block} [ELSE block] END | 
     FOR Name `=´ exp `,´ exp [`,´ exp] DO block END | 
     FOR namelist IN explist DO block END | 
     function funcname funcbody | 
     local function Name funcbody | 
     local namelist [`=´ explist] 

laststat ::= return [explist] | break

funcname ::= Name {`.´ Name} [`:´ Name]

varlist ::= var {`,´ var}

var ::=  Name | prefixexp `[´ exp `]´ | prefixexp `.´ Name 

namelist ::= Name {`,´ Name}

explist ::= {exp `,´} exp

exp ::=  nil | false | true | Number | String | `...´ | function | 
     prefixexp | tableconstructor | exp binop exp | unop exp 

prefixexp ::= var | functioncall | `(´ exp `)´

functioncall ::=  prefixexp args | prefixexp `:´ Name args 

args ::=  `(´ [explist] `)´ | tableconstructor | String 

function ::= function funcbody

funcbody ::= `(´ [parlist] `)´ block END

parlist ::= namelist [`,´ `...´] | `...´

tableconstructor ::= `{´ [fieldlist] `}´

fieldlist ::= field {fieldsep field} [fieldsep]

field ::= `[´ exp `]´ `=´ exp | Name `=´ exp | exp

fieldsep ::= `,´ | `;´

binop ::= `+´ | `-´ | `*´ | `/´ | `^´ | `%´ | `..´ | 
     `<´ | `<=´ | `>´ | `>=´ | `==´ | `~=´ | 
     and | or

unop ::= `-´ | not | `#´