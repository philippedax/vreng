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
     ENTIER = 258,
     REEL = 259,
     NOM = 260,
     CHAINE = 261,
     TRUE = 262,
     FALSE = 263,
     VAR = 264,
     BEGINKEY = 265,
     END = 266,
     IMPORT = 267,
     IF = 268,
     ELSE = 269,
     WHILE = 270,
     NEW = 271,
     COMPOUND = 272,
     CLASSE = 273,
     EXTENDS = 274,
     CONSTRUCTOR = 275,
     THIS = 276,
     NOT = 277,
     AND = 278,
     OR = 279,
     EGAL = 280,
     PLUS = 281,
     MOINS = 282,
     MULT = 283,
     DIV = 284,
     MOD = 285,
     ABS = 286,
     SUP = 287,
     INF = 288,
     SUPEGAL = 289,
     INFEGAL = 290,
     EGALEGAL = 291,
     DIFFERENT = 292,
     PARENTOUVR = 293,
     PARENTFERM = 294,
     CROCHOUVR = 295,
     CROCHFERM = 296,
     ACCOUVR = 297,
     ACCFERM = 298,
     PTVIRG = 299,
     VIRGULE = 300,
     POINT = 301
   };
#endif
/* Tokens.  */
#define ENTIER 258
#define REEL 259
#define NOM 260
#define CHAINE 261
#define TRUE 262
#define FALSE 263
#define VAR 264
#define BEGINKEY 265
#define END 266
#define IMPORT 267
#define IF 268
#define ELSE 269
#define WHILE 270
#define NEW 271
#define COMPOUND 272
#define CLASSE 273
#define EXTENDS 274
#define CONSTRUCTOR 275
#define THIS 276
#define NOT 277
#define AND 278
#define OR 279
#define EGAL 280
#define PLUS 281
#define MOINS 282
#define MULT 283
#define DIV 284
#define MOD 285
#define ABS 286
#define SUP 287
#define INF 288
#define SUPEGAL 289
#define INFEGAL 290
#define EGALEGAL 291
#define DIFFERENT 292
#define PARENTOUVR 293
#define PARENTFERM 294
#define CROCHOUVR 295
#define CROCHFERM 296
#define ACCOUVR 297
#define ACCFERM 298
#define PTVIRG 299
#define VIRGULE 300
#define POINT 301




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 71 "vrel.y"
{
        int 	          lineNo ;
        int 	          valeurint ;
        float 	          valeurfloat ;
        char 	          *chaine ;
        Programme         *programme ;
        Decl_var	  *declaration_var ;
        Decl_var_liste    *declaration_var_liste ;
        Vrel              *declaration_classe ;
        Constructeur      *constructeur ;
        Env_constr 	  *definition_classe ;
        Instruction       *instruction ;
        Instruction_liste *instruction_liste ;
        Instruction_liste *consequence ;
        Instruction_liste *affectation_classe_liste ;
        Alternative       *instructionif ;
        Affectation       *affectation ;
        Creation          *creation ;
        Data_liste        *dataliste ;
        Data              *data ;
        Data    	  *operation ;
        Data		  *test ;
        Data		  *comparaison ;
        Variable          *variable ;
        Nombre		  *nombre ;
        Nom_liste         *noms ; 
}
/* Line 1529 of yacc.c.  */
#line 169 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

