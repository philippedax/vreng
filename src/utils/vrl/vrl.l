%{

#include "vrl.hpp"
#include "y.tab.h"

extern "C" {
  char *strdup(const char *);
}

extern void yyerror(char *s);

/* redifinition int yylineno = 1; */
int yypos = 1;

#define RETURN yypos += strlen(yytext); return

extern Fichier *fvrl;

%}

%option nounput

%%

"# " { while ((char)yyinput() != '\n'); yylineno++; yypos = 1; }
"//" { while ((char)yyinput() != '\n'); yylineno++; yypos = 1; }

"struct"              { yylval.lineNo = yylineno; RETURN STRUCTKEY; }
"{"                   { yylval.lineNo = yylineno; RETURN LEFTBRACEKEY; }
"}"                   { yylval.lineNo = yylineno; RETURN RIGHTBRACEKEY; }
"var"                 { yylval.lineNo = yylineno; RETURN VARKEY; }
";"                   { yylval.lineNo = yylineno; RETURN SEMICOLONKEY; }
","                   { yylval.lineNo = yylineno; RETURN COMMAKEY; }
"["                   { yylval.lineNo = yylineno; RETURN LEFTBRACKETKEY; }
"]"                   { yylval.lineNo = yylineno; RETURN RIGHTBRACKETKEY; }
"constructor"         { yylval.lineNo = yylineno; RETURN CONSTRUCTORKEY; }
"new"                 { yylval.lineNo = yylineno; RETURN NEWKEY; }
"("                   { yylval.lineNo = yylineno; RETURN LEFTPARENKEY; }
")"                   { yylval.lineNo = yylineno; RETURN RIGHTPARENKEY; }
"="                   { yylval.lineNo = yylineno; RETURN EQUALKEY; }
":"                   { yylval.lineNo = yylineno; RETURN COLONKEY; }
"main"                { yylval.lineNo = yylineno; RETURN MAINKEY; }
"function"            { yylval.lineNo = yylineno; RETURN FUNCTIONKEY; }
"call"                { yylval.lineNo = yylineno; RETURN CALLKEY; }
"contructor"          { yylval.lineNo = yylineno; RETURN CONSTRUCTORKEY; }
"return"              { yylval.lineNo = yylineno; RETURN RETURNKEY; }
"if"                  { yylval.lineNo = yylineno; RETURN IFKEY; }
"else"                { yylval.lineNo = yylineno; RETURN ELSEKEY; }
"for"                 { yylval.lineNo = yylineno; RETURN FORKEY; }
"."                   { yylval.lineNo = yylineno; RETURN DOTKEY; }
"+"                   { yylval.lineNo = yylineno; RETURN PLUSKEY; }
"-"                   { yylval.lineNo = yylineno; RETURN MINUSKEY; }
"*"                   { yylval.lineNo = yylineno; RETURN MULTKEY; }
"/"                   { yylval.lineNo = yylineno; RETURN DIVKEY; }
"abs"                 { yylval.lineNo = yylineno; RETURN ABSKEY; }
">"                   { yylval.lineNo = yylineno; RETURN SUPKEY; }
"<"                   { yylval.lineNo = yylineno; RETURN INFKEY; }
">="                  { yylval.lineNo = yylineno; RETURN SUPEQUALKEY; }
"<="                  { yylval.lineNo = yylineno; RETURN INFEQUALKEY; }
"=="                  { yylval.lineNo = yylineno; RETURN EQUALEQUALKEY; }
"!="                  { yylval.lineNo = yylineno; RETURN EXCLAMATIONEQUALKEY; }
"!"                   { yylval.lineNo = yylineno; RETURN EXCLAMATIONKEY; }
"||"                  { yylval.lineNo = yylineno; RETURN ORKEY; }
"&&"                  { yylval.lineNo = yylineno; RETURN ANDKEY; }

[a-zA-Z][a-zA-Z_0-9]* { yylval.chaine = strdup(yytext); RETURN NAME; }
[1-9][0-9]*           { yylval.valeurint = (int)atoi(yytext); RETURN INTEGER; }
"0"		      { yylval.valeurint = (int)atoi(yytext); RETURN INTEGER; }
[+-]?[0-9]*"."[0-9]+  { yylval.valeurfloat = (float)atof(yytext); RETURN FLOATNUMBER; }
"\""[^\n\"]*"\""      { yylval.chaine = strdup(yytext); RETURN STRING; }

[\ \t]                { }
\n                    { yylineno++; yypos = 1; }

. {
	int i;
	for (i=1;i<10;i++)
	  if (!feof(yyin)) {
	    yytext[i] = (char)yyinput();
	    if (yytext[i] == '\n')
	      yytext[i] = '\0';
	  };
	yytext[i] = '\0';
	yyerror((char *)"undefined token");
}

%%

int yywrap()
{
  return 1;
}

/*** in vrl.cpp
// dax
int main(int argc, char **argv)
{
  ++argv, --argc; // skip over program name
  if ( argc > 0 ) {
    yyin = fopen( argv[0], "r" );
    printf("input = %s\n", argv[0]);
  }
  else {
    yyin = stdin;
    printf("input = stdin\n");
  }
  fvrl = yyin;

  if ( argc > 1 ) {
    fvre = fopen( argv[1], "w" );
    printf("output = %s\n", argv[1]);
  }
  else {
    fvre = stdout;
    printf("output = stdout\n");
  }

  while (yylex());
}
***/
