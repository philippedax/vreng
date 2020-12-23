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
     STRUCTKEY = 258,
     LEFTBRACEKEY = 259,
     RIGHTBRACEKEY = 260,
     VARKEY = 261,
     SEMICOLONKEY = 262,
     COMMAKEY = 263,
     LEFTBRACKETKEY = 264,
     RIGHTBRACKETKEY = 265,
     CONSTRUCTORKEY = 266,
     NEWKEY = 267,
     LEFTPARENKEY = 268,
     RIGHTPARENKEY = 269,
     EQUALKEY = 270,
     PERIODKEY = 271,
     COLONKEY = 272,
     MAINKEY = 273,
     FUNCTIONKEY = 274,
     CALLKEY = 275,
     RETURNKEY = 276,
     IFKEY = 277,
     ELSEKEY = 278,
     FORKEY = 279,
     DOTKEY = 280,
     PLUSKEY = 281,
     MINUSKEY = 282,
     MULTKEY = 283,
     DIVKEY = 284,
     ABSKEY = 285,
     SUPKEY = 286,
     INFKEY = 287,
     SUPEQUALKEY = 288,
     INFEQUALKEY = 289,
     EQUALEQUALKEY = 290,
     EXCLAMATIONEQUALKEY = 291,
     EXCLAMATIONKEY = 292,
     ORKEY = 293,
     ANDKEY = 294,
     NAME = 295,
     STRING = 296,
     INTEGER = 297,
     FLOATNUMBER = 298
   };
#endif
/* Tokens.  */
#define STRUCTKEY 258
#define LEFTBRACEKEY 259
#define RIGHTBRACEKEY 260
#define VARKEY 261
#define SEMICOLONKEY 262
#define COMMAKEY 263
#define LEFTBRACKETKEY 264
#define RIGHTBRACKETKEY 265
#define CONSTRUCTORKEY 266
#define NEWKEY 267
#define LEFTPARENKEY 268
#define RIGHTPARENKEY 269
#define EQUALKEY 270
#define PERIODKEY 271
#define COLONKEY 272
#define MAINKEY 273
#define FUNCTIONKEY 274
#define CALLKEY 275
#define RETURNKEY 276
#define IFKEY 277
#define ELSEKEY 278
#define FORKEY 279
#define DOTKEY 280
#define PLUSKEY 281
#define MINUSKEY 282
#define MULTKEY 283
#define DIVKEY 284
#define ABSKEY 285
#define SUPKEY 286
#define INFKEY 287
#define SUPEQUALKEY 288
#define INFEQUALKEY 289
#define EQUALEQUALKEY 290
#define EXCLAMATIONEQUALKEY 291
#define EXCLAMATIONKEY 292
#define ORKEY 293
#define ANDKEY 294
#define NAME 295
#define STRING 296
#define INTEGER 297
#define FLOATNUMBER 298




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 20 "vrl.y"
{
	int                            lineNo ;
	char                           *chaine ;
	int                            valeurint ;
	float                          valeurfloat ;
	Table                          *table ;
	Entree                         *entree ;
	EntreeFloat                    *entreefloat ;
	EntreeString                   *entreestring ;
	EntreeObjet                    *entreeobjet ;
	PointeurPile                   *pointeurpile ;
	Pointeur                       *pointeur ;
	Fichier                        *fichier ;
	StructureListe                 *structureliste ;
	Structure                      *structure ;
	StructureHeritee               *structureheritee ;
	FonctionListe                  *fonctionliste ;
	Fonction                       *fonction ;
	PointDEntree                   *pointdentree ;
	Constructeur                   *constructeur ;
	InstructionListe               *instructionliste ;
	Instruction                    *instruction ;
	AppelDeConstructeur            *appeldeconstructeur ;
	Statement                      *statement ;
	Retour                         *retour ;
	BoucleIf                       *boucleif ;
	BoucleFor                      *bouclefor ;
	DeclarationListe               *declarationliste;
	Declaration                    *declaration ;
	DeclarationSimple              *declarationsimple ;
	DeclarationComplexe            *declarationcomplexe ;
	DeclarationComplexeChaine      *declarationcomplexechaine ;
	DeclarationComplexeExpression  *declarationcomplexeexpression ;
	DeclarationComplexeNouvelObjet *declarationcomplexenouvelobjet ;
	Assignation                    *assignation ;
	AssignationChaine	       *assignationchaine ;
	AssignationExpression	       *assignationexpression ;
	AssignationVariable	       *assignationvariable ;
	AssignationNouvelObjet         *assignationnouvelobjet ;
	NouvelObjet                    *nouvelobjet ;
	ExpressionListe                *expressionliste ;
	Expression                     *expression ;
	ExpressionCalculable	       *calculableexpression ;
	Nombre                         *nombre ;
	AppelDeFonction                *appeldefonction ;
	Variable                       *variable ;
	VariableSimpleListe            *variablesimpleliste ;
	VariableSimple                 *variablesimple ;	
	NomListe                       *nomliste ;
	Nom                            *nom ;
	NomDeTableau                   *nomdetableau ;
	VariableComplexe               *variablecomplexe ;
	Operation                      *operation ;
	OperationBinaire               *operationbinaire ;
	Addition                       *addition ;
	Soustraction                   *soustraction ;
	Multiplication                 *multiplication ;
	Division                       *division ;
	OperationUnaire                *operationunaire ;
	Negation                       *negation ;
	Absolu                         *absolu ;
	ExpressionLogiqueListe         *expressionlogiqueliste ;
	ExpressionLogique              *expressionlogique ;
	Comparaison                    *comparaison ;
	Superieur                      *superieur ;
	Inferieur                      *inferieur ;
	SuperieurOuEgal                *superieurouegal ;
	InferieurOuEgal                *inferieurouegal ;
	EgalEgal                       *egalegal ;
	Different                      *different ;
	OperationBinaireLogique        *operationbinairelogique ;
	OuLogique                      *oulogique ;
	EtLogique                      *etlogique ;
	OperationUnaireLogique         *operationunairelogique ;
	NegationLogique                *negationlogique ;
}
/* Line 1529 of yacc.c.  */
#line 212 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

