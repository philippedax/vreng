%{
/****************************************************************************
 *                             VREL COMPONENTS                              *
 *                                                                          *
 *                           Copyright (C) 2000                             *
 *     Yanneck Chevalier, Pascal Belin, Alexis Jeannerod, Julien Dauphin    *
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 ****************************************************************************/

#include "vrel.hpp"

#include "program.hpp"
#include "namelist.hpp"
#include "declvar.hpp"
#include "decllist.hpp"
#include "instlist.hpp"
#include "affectation.hpp"
#include "alternative.hpp"
#include "while.hpp"
#include "new.hpp"
#include "datalist.hpp"
#include "var.hpp"
#include "number.hpp"
#include "false.hpp"
#include "true.hpp"
#include "str.hpp"
#include "add.hpp"
#include "sub.hpp"
#include "mul.hpp"
#include "div.hpp"
#include "mod.hpp"
#include "abs.hpp"
#include "opp.hpp"
#include "egal.hpp"
#include "dif.hpp"
#include "sup.hpp"
#include "inf.hpp"
#include "supegal.hpp"
#include "infegal.hpp"
#include "and.hpp"
#include "or.hpp"
#include "not.hpp"
#include "compound.hpp"
#include "extends.hpp"
#include "constructor.hpp"
#include "envconstr.hpp"


extern char *yytext;           // sortie de lex
extern int yylex();            // analyseur lexical
extern void yyerror(char *s);  // traitement des "parse error"
extern int yylineno;

extern "C" {
 extern void free(void *p);
}				// declaration de la fonction free pour c++
extern Programme *prog;		// Pointeur sur le programme.

%}


%union{
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


%token <valeurint>   ENTIER
%token <valeurfloat> REEL
%token <chaine> NOM
%token <chaine> CHAINE
%token <lineNo> TRUE
%token <lineNo> FALSE
%token <lineNo> VAR
%token <lineNo> BEGINKEY
%token <lineNo> END
%token <lineNo> IMPORT
%token <lineNo> IF
%token <lineNo> ELSE
%token <lineNo> WHILE
%token <lineNo> NEW
%token <lineNo> COMPOUND
%token <lineNo> CLASSE
%token <lineNo> EXTENDS
%token <lineNo> CONSTRUCTOR
%token <lineNo> THIS
%token <lineNo> NOT
%token <lineNo> AND
%token <lineNo> OR
%token <lineNo> EGAL
%token <lineNo> PLUS
%token <lineNo> MOINS
%token <lineNo> MULT
%token <lineNo> DIV
%token <lineNo> MOD
%token <lineNo> ABS
%token <lineNo> SUP
%token <lineNo> INF
%token <lineNo> SUPEGAL
%token <lineNo> INFEGAL
%token <lineNo> EGALEGAL
%token <lineNo> DIFFERENT
%token <lineNo> PARENTOUVR
%token <lineNo> PARENTFERM
%token <lineNo> CROCHOUVR
%token <lineNo> CROCHFERM
%token <lineNo> ACCOUVR
%token <lineNo> ACCFERM
%token <lineNo> PTVIRG
%token <lineNo> VIRGULE
%token <lineNo> POINT

%type <programme>	   	PROGRAMME
%type <declaration_var>	   	DECLARATION_VAR
%type <declaration_var_liste>   DECLARATION_VAR_LISTE
%type <declaration_classe> 	DECLARATION_CLASSE
%type <definition_classe>	DEFINITION_CLASSE
%type <constructeur>            CONSTRUCTEUR
%type <affectation_classe_liste>AFFECTATION_CLASSE_LISTE
%type <instruction_liste>   	INSTRUCTION_LISTE
%type <instruction>	   	INSTRUCTION
%type <instructionif>	   	INSTRUCTION_IF
%type <consequence>	   	CONSEQUENCE
%type <affectation>	   	AFFECTATION
%type <data>                    DATA
%type <operation>          	OPERATION
%type <test> 		   	TEST
%type <comparaison>	   	COMPARAISON
%type <variable>	   	VARIABLE
%type <dataliste>	   	DATALISTE
%type <noms>		   	NOMS
%type <nombre>             	NOMBRE
%type <vide>	           	VIDE

%right EGAL
%left AND OR NOT
%left SUP INF EGALEGAL SUPEGAL INFEGAL DIFFERENT
%left PLUS MOINS
%left MULT DIV MOD

%start PROGRAMME

%% 

PROGRAMME : VIDE
                { $$ = new Programme() ; prog = $$ ; /* Nouveau programme */ }
	| PROGRAMME IMPORT CHAINE
		{ $1->import($3) ; /* Importation d'un autre fichier */ }
	| PROGRAMME DECLARATION_VAR
                { $1->adddeclarvar($2) ;  /* Declaration de variable globale effectuee */ }
	| PROGRAMME DECLARATION_CLASSE
                { /* Declaration de classe effectuee */ }
	| PROGRAMME BEGINKEY ACCOUVR  DECLARATION_VAR_LISTE INSTRUCTION_LISTE ACCFERM END
		{ $$->addmain($4, $5) ; /* Fin du main */ }

DECLARATION_VAR : VAR VARIABLE PTVIRG
		{ $$ = new Decl_var($2) ; /* Declaration simple */ }
	| VAR VARIABLE EGAL DATA PTVIRG
		{ $$ = new Decl_var($2, $4) ; /* Declaration et affectation */ } 

DECLARATION_VAR_LISTE : VIDE
		{ $$ = new Decl_var_liste() ; /* Debut des declarations de variables */ }
	| DECLARATION_VAR_LISTE DECLARATION_VAR
		{ $1->adddeclarvar($2) ;  /* Declaration de variable effectuee */ }

DECLARATION_CLASSE : CLASSE NOM EXTENDS NOM ACCOUVR AFFECTATION_CLASSE_LISTE ACCFERM 
		{ $$ = new Class_extends($2, $4, $6) ; /* Classe heritee */ }
	| CLASSE NOM COMPOUND ACCOUVR DECLARATION_VAR_LISTE DEFINITION_CLASSE ACCFERM
		{ $$ = new Compound($2, $5, $6) ; /* Classe construite */ }

DEFINITION_CLASSE : VIDE
		{ $$ = new Env_constr() ; /* Debut de la definition de la classe */ }
        | DEFINITION_CLASSE CONSTRUCTEUR
                { $1->addConstr($2) ; /* Nouveau constructeur rentre */ }

CONSTRUCTEUR : CONSTRUCTOR PARENTOUVR NOMS PARENTFERM ACCOUVR DECLARATION_VAR_LISTE INSTRUCTION_LISTE ACCFERM
		{ $$ = new Constructeur($3, $6, $7) ;  /* Fin du constructeur */ }

AFFECTATION_CLASSE_LISTE : VIDE
		{ $$ = new Instruction_liste() ; /* Debut de l'affectation de classe */ }
	| AFFECTATION_CLASSE_LISTE AFFECTATION
		{ $1->addInstruction($2) ; /* Affectation de classe finie */ }

INSTRUCTION_LISTE : VIDE
		{ $$ = new Instruction_liste() ; /* Debut des instructions */ }
	| INSTRUCTION_LISTE INSTRUCTION
		{ $$->addInstruction($2) ;  /* Fin de l'instruction */ }

INSTRUCTION : AFFECTATION
		{ $$ = $1 ; /* Fin de l'affectation */ }
	| WHILE PARENTOUVR TEST PARENTFERM CONSEQUENCE
		{ $$ = new Boucle_while($3, $5) ; /* Fin de la boucle_while */ }
	| INSTRUCTION_IF
		{ $$ = $1 ;  /* Fin du test_if */ }
	| NEW NOM PARENTOUVR DATALISTE PARENTFERM PTVIRG
		{ $$ = new Creation($2, $4) ;  /* Fin du nouvel objet */ }
	| VARIABLE EGAL NEW NOM PARENTOUVR DATALISTE PARENTFERM PTVIRG
		{ $$ = new Creation($1, $4, $6) ; /* Nouvel objet cree */ }

INSTRUCTION_IF : IF PARENTOUVR TEST PARENTFERM CONSEQUENCE ELSE CONSEQUENCE
		{ $$ = new Alternative($3, $5, $7) ; /* If then else */ }
	| IF PARENTOUVR TEST PARENTFERM CONSEQUENCE
		{ $$ = new Alternative($3, $5) ; /* If then */ }

CONSEQUENCE : ACCOUVR INSTRUCTION_LISTE ACCFERM
		{ $$ = $2 ; /* Fin des consequences */ }

AFFECTATION : VARIABLE EGAL DATA PTVIRG
		{ $$ = new Affectation($1, $3) ; /* Affectation effectuee */ }
         | NOM POINT NOM EGAL DATA PTVIRG
		{ $$ = new Affectation($1, $3, $5) ; /* Element d'un objet */ }

DATA :  VARIABLE
		{ $$ = $1 ; /* Data=variable */ }
	| NOMBRE
		{ $$ = $1 ; /* Data=nombre */ }
        | CHAINE
		{ $$ = new Chaine($1) ;  /* Data=chaine de caractères */ }
	| PARENTOUVR DATA PARENTFERM
		{ $$ = $2 ; /* (data) */ }
	| OPERATION
		{ $$ = $1 ; /* Data=operation */ }
        | TRUE
		{ $$ = new Vrai() ; /* Data=vrai */ }
        | FALSE
		{ $$ = new Faux() ; /* Data=faux */ }

OPERATION : DATA PLUS DATA
		{ $$ = new Addition      ($1, $3) ; /* Addition */ }
	| DATA MOINS DATA
		{ $$ = new Soustraction  ($1, $3) ; /* Soustraction */ }
	| DATA MULT DATA
		{ $$ = new Multiplication($1, $3) ; /* Multiplication */ }
	| DATA DIV DATA
		{ $$ = new Division      ($1, $3) ; /* Division */ }
	| DATA MOD DATA
		{ $$ = new Modulo        ($1, $3) ; /* Modulo */ }
	| ABS PARENTOUVR DATA PARENTFERM
		{ $$ = new Valabsolue($3) ; /* Valeur absolue */ }
	| MOINS DATA %prec MULT
		{ $$ = new Opposition($2) ; /* Opposee */ }

TEST : COMPARAISON 
		{ $$ = $1 ; /* Comparaison */ }
        | PARENTOUVR COMPARAISON PARENTFERM
		{ $$ = $2 ; /* Comparaison */ }
	| TEST AND TEST
		{ $$ = new And($1, $3) ; /* Test AND test */ }
	| TEST OR TEST
		{ $$ = new Or ($1, $3) ; /* Test OR test */ }
	| NOT TEST
		{ $$ = new Not($2) ;     /* NOT test */ }
	| FALSE
		{ $$ = new Faux() ; /* FALSE */ }
	| TRUE
		{ $$ = new Vrai() ; /* TRUE */ }
	| VARIABLE
		{ $$ = $1 ; }

COMPARAISON : DATA EGALEGAL DATA
		{ $$ = new Egalite  ($1, $3) ; /* Egal-egal */ }
	| DATA SUP DATA
		{ $$ = new Superieur($1, $3) ; /* Superieur */ }
	| DATA INF DATA
		{ $$ = new Inferieur($1, $3) ; /* Inferieur */ }
	| DATA SUPEGAL DATA
		{ $$ = new Supegal  ($1, $3) ; /* Superieur ou Egal */ }
	| DATA INFEGAL DATA
		{ $$ = new Infegal  ($1, $3) ; /* Inferieur ou Egal */ }
	| DATA DIFFERENT DATA
		{ $$ = new Different($1, $3) ; /* Different */ }

VARIABLE : NOM
		{ $$ = new Variable($1) ; /* Variable simple */ }
	| NOM CROCHOUVR DATA CROCHFERM
		{/* $$ = new Variable($1, $3) ; */ /* Tableau : non implemente */ }

NOMS : VIDE
		{ $$ = new Nom_liste() ; /* Liste vide */ }
	| NOM
		{ $$ = new Nom_liste() ; $$->ajouter($1) ; /* Liste simple */ }
	| NOMS VIRGULE NOM
		{ $1->ajouter($3) ; /* Variable simple */ }

DATALISTE : VIDE
		{ $$ = new Data_liste() ; /* Liste2 vide */ }
	| DATA
		{ $$ = new Data_liste() ; $$->ajouter($1) ; /* Liste simple */ }
	| DATALISTE VIRGULE DATA
		{ $1->ajouter($3) ; /* Variable simple */ }

NOMBRE : ENTIER
		{ $$ = new Nombre($1) ; /* Entier */ }	
	| REEL
		{ $$ = new Nombre($1) ; /* Reel */ }

VIDE : {}

%%

void yyerror(char *s)
{
  printf("syntax error: %s\n", s);
  printf("File: %s, line %d: \"%s\"\n", vrelfile, yylineno, yytext);
  exit(0);
}
