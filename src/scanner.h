#ifndef SCANNER_H
#define SCANNER_H

#include <stdbool.h>

enum TokenEnum {

    // con valor
    TK_ID               // identificador
    TK_NUMBER_INT       // int
    TK_NUMBER_DOUBLE    // double
    TK_STRING           // string
    TK_BOOLEAN          // booleano
    
    // operadores
    TK_OP_PLUS          // +
    TK_OP_MINUS         // -
    TK_OP_TIMES         // *
    TK_OP_DIVIDED       // /
    TK_OP_MOD           // %
    TK_OP_EXP           // ^
    TK_OP_HASH          // #
    TK_OP_EQUALS        // ==
    TK_OP_DIFF          // ~=
    TK_OP_MIN_EQUALS    // <=
    TK_OP_GRT_EQUALS    // >=
    TK_OP_MIN           // <
    TK_OP_GRT           // >
    TK_OP_ASSIGN        // =
    TK_OP_OPEN_PAREN    // (
    TK_OP_CLOS_PAREN    // )
    TK_OP_OPEN_BRACE    // {
    TK_OP_CLOS_BRACE    // }
    TK_OP_OPEN_BRACK    // [
    TK_OP_CLOS_BRACK    // ]
    TK_OP_SEMICOLON     // ;
    TK_OP_COLON         // :
    TK_OP_COMA          // ,
    TK_OP_DOT           // .
    TK_OP_DOTDOT        // ..
    TK_OP_ELIPSIS       // ...

    // keywords
    TK_KW_BREAK         // break
    TK_KW_DO            // do
    TK_KW_ELSE          // else
    TK_KW_ELSEIF        // elseif
    TK_KW_END           // end
    TK_KW_FOR           // for
    TK_KW_FUNCTION      // function
    TK_KW_IF            // if
    TK_KW_IN            // in
    TK_KW_LOCAL         // local
    TK_KW_NIL           // nil
    TK_KW_NOT           // not
    TK_KW_AND           // and
    TK_KW_OR            // or
    TK_KW_WHILE         // while
    TK_KW_REPEAT        // repeat
    TK_KW_RETURN        // return
    TK_KW_THEN          // then
    TK_KW_UNTIL         // until
};

struct Token {
    TokenEnum type;
    int line_no;
    int value_int;
    double value_double;
    bool value_boolean;
    std::string value_string;
};

#endif