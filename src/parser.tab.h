/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TK_ID = 258,
     TK_NUMBER_INT = 259,
     TK_NUMBER_DOUBLE = 260,
     TK_STRING = 261,
     TK_BOOLEAN = 262,
     TK_OP_PLUS = 263,
     TK_OP_MINUS = 264,
     TK_OP_TIMES = 265,
     TK_OP_DIVIDED = 266,
     TK_OP_MOD = 267,
     TK_OP_EXP = 268,
     TK_OP_HASH = 269,
     TK_OP_EQUALS = 270,
     TK_OP_DIFF = 271,
     TK_OP_MIN_EQUALS = 272,
     TK_OP_GRT_EQUALS = 273,
     TK_OP_MIN = 274,
     TK_OP_GRT = 275,
     TK_OP_ASSIGN = 276,
     TK_OP_OPEN_PAREN = 277,
     TK_OP_CLOS_PAREN = 278,
     TK_OP_OPEN_BRACE = 279,
     TK_OP_CLOS_BRACE = 280,
     TK_OP_OPEN_BRACK = 281,
     TK_OP_CLOS_BRACK = 282,
     TK_OP_SEMICOLON = 283,
     TK_OP_COLON = 284,
     TK_OP_COMA = 285,
     TK_OP_DOT = 286,
     TK_OP_DOTDOT = 287,
     TK_OP_ELIPSIS = 288,
     TK_KW_BREAK = 289,
     TK_KW_DO = 290,
     TK_KW_ELSE = 291,
     TK_KW_ELSEIF = 292,
     TK_KW_END = 293,
     TK_KW_FOR = 294,
     TK_KW_FUNCTION = 295,
     TK_KW_IF = 296,
     TK_KW_IN = 297,
     TK_KW_LOCAL = 298,
     TK_KW_NIL = 299,
     TK_KW_NOT = 300,
     TK_KW_AND = 301,
     TK_KW_OR = 302,
     TK_KW_WHILE = 303,
     TK_KW_REPEAT = 304,
     TK_KW_RETURN = 305,
     TK_KW_THEN = 306,
     TK_KW_UNTIL = 307
   };
#endif
/* Tokens.  */
#define TK_ID 258
#define TK_NUMBER_INT 259
#define TK_NUMBER_DOUBLE 260
#define TK_STRING 261
#define TK_BOOLEAN 262
#define TK_OP_PLUS 263
#define TK_OP_MINUS 264
#define TK_OP_TIMES 265
#define TK_OP_DIVIDED 266
#define TK_OP_MOD 267
#define TK_OP_EXP 268
#define TK_OP_HASH 269
#define TK_OP_EQUALS 270
#define TK_OP_DIFF 271
#define TK_OP_MIN_EQUALS 272
#define TK_OP_GRT_EQUALS 273
#define TK_OP_MIN 274
#define TK_OP_GRT 275
#define TK_OP_ASSIGN 276
#define TK_OP_OPEN_PAREN 277
#define TK_OP_CLOS_PAREN 278
#define TK_OP_OPEN_BRACE 279
#define TK_OP_CLOS_BRACE 280
#define TK_OP_OPEN_BRACK 281
#define TK_OP_CLOS_BRACK 282
#define TK_OP_SEMICOLON 283
#define TK_OP_COLON 284
#define TK_OP_COMA 285
#define TK_OP_DOT 286
#define TK_OP_DOTDOT 287
#define TK_OP_ELIPSIS 288
#define TK_KW_BREAK 289
#define TK_KW_DO 290
#define TK_KW_ELSE 291
#define TK_KW_ELSEIF 292
#define TK_KW_END 293
#define TK_KW_FOR 294
#define TK_KW_FUNCTION 295
#define TK_KW_IF 296
#define TK_KW_IN 297
#define TK_KW_LOCAL 298
#define TK_KW_NIL 299
#define TK_KW_NOT 300
#define TK_KW_AND 301
#define TK_KW_OR 302
#define TK_KW_WHILE 303
#define TK_KW_REPEAT 304
#define TK_KW_RETURN 305
#define TK_KW_THEN 306
#define TK_KW_UNTIL 307




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

