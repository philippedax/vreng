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
#include "inst.hpp"
#include "instlist.hpp"
#include "affectation.hpp"
#include "alternative.hpp"
#include "new.hpp"
#include "datalist.hpp"
#include "data.hpp"
#include "var.hpp"
#include "number.hpp"
#include "class.hpp"
#include "constructor.hpp"
#include "envconstr.hpp"

#include "y.tab.h"


extern "C" { char *strdup(const char *); }
extern void yyerror(char *s);

extern int yylineno;

#define YY_INPUT(buf, result, max_size) \
  {\
  int c = getc(yyin);\
  result = (c==EOF)? YY_NULL : (buf[0] = c,1);\
  }

#define RETURN return

%}


/* Tokens du langage */
%%


"/*" {
  int c;
  for ( ; ; ) {
    while ((c=yyinput()) != '*' && c != EOF && c!= '\n');
    if (c == '*') {
      if ( (c=yyinput()) == '/' )
        break;
    }
    if (c == EOF) { yywrap(); break; }
    if (c == '\n') yylineno++;
  }
}

"//" { while ((char)yyinput() != '\n'); yylineno++; }

"var"                 { yylval.lineNo = yylineno; RETURN VAR; }
"begin"               { yylval.lineNo = yylineno; RETURN BEGINKEY; }
"end"                 { yylval.lineNo = yylineno; RETURN END; }
"import"              { yylval.lineNo = yylineno; RETURN IMPORT; }
"if"                  { yylval.lineNo = yylineno; RETURN IF; }
"else"                { yylval.lineNo = yylineno; RETURN ELSE; }
"while"               { yylval.lineNo = yylineno; RETURN WHILE; }
"new"                 { yylval.lineNo = yylineno; RETURN NEW; }
"compound"            { yylval.lineNo = yylineno; RETURN COMPOUND; }
"class"               { yylval.lineNo = yylineno; RETURN CLASSE; }
"extends"             { yylval.lineNo = yylineno; RETURN EXTENDS; }
"constructor"         { yylval.lineNo = yylineno; RETURN CONSTRUCTOR; }
"="                   { yylval.lineNo = yylineno; RETURN EGAL; }
"+"                   { yylval.lineNo = yylineno; RETURN PLUS; }
"-"                   { yylval.lineNo = yylineno; RETURN MOINS; }
"*"                   { yylval.lineNo = yylineno; RETURN MULT; }
"/"                   { yylval.lineNo = yylineno; RETURN DIV; }
"mod"                 { yylval.lineNo = yylineno; RETURN MOD; }
"abs"                 { yylval.lineNo = yylineno; RETURN ABS; }
">"                   { yylval.lineNo = yylineno; RETURN SUP; }
"<"                   { yylval.lineNo = yylineno; RETURN INF; }
">="                  { yylval.lineNo = yylineno; RETURN SUPEGAL; }
"<="                  { yylval.lineNo = yylineno; RETURN INFEGAL; }
"=="                  { yylval.lineNo = yylineno; RETURN EGALEGAL; }
"!="                  { yylval.lineNo = yylineno; RETURN DIFFERENT; }
"!"                   { yylval.lineNo = yylineno; RETURN NOT; }
"&&"                  { yylval.lineNo = yylineno; RETURN AND; }
"||"                  { yylval.lineNo = yylineno; RETURN OR; }
"("                   { yylval.lineNo = yylineno; RETURN PARENTOUVR; }
")"                   { yylval.lineNo = yylineno; RETURN PARENTFERM; }
"["                   { yylval.lineNo = yylineno; RETURN CROCHOUVR; }
"]"                   { yylval.lineNo = yylineno; RETURN CROCHFERM; }
"{"                   { yylval.lineNo = yylineno; RETURN ACCOUVR; }
"}"                   { yylval.lineNo = yylineno; RETURN ACCFERM; }
";"                   { yylval.lineNo = yylineno; RETURN PTVIRG; }
","                   { yylval.lineNo = yylineno; RETURN VIRGULE; }
"."                   { yylval.lineNo = yylineno; RETURN POINT; }

"TRUE"                { RETURN TRUE; }
"FALSE"               { RETURN FALSE; }

[1-9][0-9]*           { yylval.valeurint = (int)atoi(yytext); RETURN ENTIER; }
"0"                   { yylval.valeurint = (int)atoi(yytext); RETURN ENTIER; }
[+-]?[0-9]*"."[0-9]+  { yylval.valeurfloat = (float)atof(yytext); RETURN REEL; }
[a-zA-Z][a-zA-Z_0-9]* { yylval.chaine = strdup(yytext); RETURN NOM; }

"\""[^\n\"]*"\"" {
  int i = 0;
  yylval.chaine = strdup(yytext);
  while (yylval.chaine[i+1] != '\"') {
    yylval.chaine[i] = yylval.chaine[i+1];
    i++;
  }
  yylval.chaine[i] = '\0';
  RETURN CHAINE;
}

[\ \t] {}
\n { yylineno++; }

. {
  int i;
  for (i=1; i<10; i++) {
    if (!feof(yyin)) {
      yytext[i] = (char)yyinput();
      if (yytext[i] == '\n')
        yytext[i] = '\0';
    }
  }
  yytext[i] = '\0';
  yyerror("caractere non reconnu");
}

%%


/* traitement du EOF */
int yywrap()
{
  /*
  si fichier initial, return(1) sinon, return(0)
  decrementation du compteur + repositionnement du pointeur
   */

  if (comptfichier > 0) {
    comptfichier--;
    fclose(yyin);
    yylineno = tablinenumber[comptfichier];
    strcpy(vrelfile, tabvrelfile[comptfichier]);
    yyin = tabyyin[comptfichier];
    return 0;
  }
  else {
    yyunput(0, yytext);
    return 1;
  }
}
