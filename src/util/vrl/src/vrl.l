%{

#include "vrl.hpp"
#include "y.tab.h"

extern "C" {
  char *strdup(const char *);
}

extern void yyerror(char *s);

int lineNumber = 1;
int charPos = 1;

#define RETURN charPos += strlen(yytext); return

FILE *fvrl;
FILE *fvre;

%}

%option nounput

%%


"# " {
	while ((char)yyinput() != '\n');
	lineNumber++;
	charPos = 1;
}

"//" {
	while ((char)yyinput() != '\n');
	lineNumber++;
	charPos = 1;
}


"struct"              { yylval.lineNo = lineNumber ; RETURN STRUCTKEY ; }
"{"                   { yylval.lineNo = lineNumber ; RETURN LEFTBRACEKEY ; }
"}"                   { yylval.lineNo = lineNumber ; RETURN RIGHTBRACEKEY ; }
"var"                 { yylval.lineNo = lineNumber ; RETURN VARKEY ; }
";"                   { yylval.lineNo = lineNumber ; RETURN SEMICOLONKEY ; }
","                   { yylval.lineNo = lineNumber ; RETURN COMMAKEY ; }
"["                   { yylval.lineNo = lineNumber ; RETURN LEFTBRACKETKEY ; }
"]"                   { yylval.lineNo = lineNumber ; RETURN RIGHTBRACKETKEY ; }
"constructor"         { yylval.lineNo = lineNumber ; RETURN CONSTRUCTORKEY ; }
"new"                 { yylval.lineNo = lineNumber ; RETURN NEWKEY ; }
"("                   { yylval.lineNo = lineNumber ; RETURN LEFTPARENKEY ; }
")"                   { yylval.lineNo = lineNumber ; RETURN RIGHTPARENKEY ; }
"="                   { yylval.lineNo = lineNumber ; RETURN EQUALKEY ; }
":"                   { yylval.lineNo = lineNumber ; RETURN COLONKEY ; }
"main"                { yylval.lineNo = lineNumber ; RETURN MAINKEY ; }
"function"            { yylval.lineNo = lineNumber ; RETURN FUNCTIONKEY ;}
"call"                { yylval.lineNo = lineNumber ; RETURN CALLKEY ;}
"contructor"          { yylval.lineNo = lineNumber ; RETURN CONSTRUCTORKEY ;}
"return"              { yylval.lineNo = lineNumber ; RETURN RETURNKEY ;}
"if"                  { yylval.lineNo = lineNumber ; RETURN IFKEY ;}
"else"                { yylval.lineNo = lineNumber ; RETURN ELSEKEY ;}
"for"                 { yylval.lineNo = lineNumber ; RETURN FORKEY ;}
"."                   { yylval.lineNo = lineNumber ; RETURN DOTKEY ;}
"+"                   { yylval.lineNo = lineNumber ; RETURN PLUSKEY ;}
"-"                   { yylval.lineNo = lineNumber ; RETURN MINUSKEY ;}
"*"                   { yylval.lineNo = lineNumber ; RETURN MULTKEY ;}
"/"                   { yylval.lineNo = lineNumber ; RETURN DIVKEY ;}
"abs"                 { yylval.lineNo = lineNumber ; RETURN ABSKEY ;}
">"                   { yylval.lineNo = lineNumber ; RETURN SUPKEY ;}
"<"                   { yylval.lineNo = lineNumber ; RETURN INFKEY ;}
">="                  { yylval.lineNo = lineNumber ; RETURN SUPEQUALKEY ;}
"<="                  { yylval.lineNo = lineNumber ; RETURN INFEQUALKEY ;}
"=="                  { yylval.lineNo = lineNumber ; RETURN EQUALEQUALKEY ;}
"!="                  { yylval.lineNo = lineNumber ; RETURN EXCLAMATIONEQUALKEY ;}
"!"                   { yylval.lineNo = lineNumber ; RETURN EXCLAMATIONKEY ;}
"||"                  { yylval.lineNo = lineNumber ; RETURN ORKEY ;}
"&&"                  { yylval.lineNo = lineNumber ; RETURN ANDKEY ;}



[a-zA-Z][a-zA-Z_0-9]* {
	yylval.chaine = strdup(yytext);
	RETURN NAME;
}

[1-9][0-9]* {
	yylval.valeurint = (int)atoi(yytext);
	RETURN INTEGER;
}

"0" {
	yylval.valeurint = (int)atoi(yytext);
	RETURN INTEGER;
}

[+-]?[0-9]*"."[0-9]+ {
	yylval.valeurfloat = (float)atof(yytext);
	RETURN FLOATNUMBER;
}

"\""[^\n\"]*"\"" {
	yylval.chaine = strdup(yytext);
	RETURN STRING;
}

[\ \t] {}

\n {
	lineNumber++;
	charPos = 1;
}

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

// dax
int main(int argc, char **argv)
{
  ++argv, --argc; /* skip over program name */
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

  yylex();
}
