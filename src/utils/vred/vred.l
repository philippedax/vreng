nl "\n"
digit [0-9]

%{
#include <stdlib.h>
#include <string.h>
#include "vre.hpp"
#include "y.tab.h"

extern int yylineno;
extern enum ItemId curtype;
%}

%option noinput
%option nounput

%%

{nl} {
  yylineno++;
}

[ \t] ;
#.*$ ;

([+-]?(([0-9]+)|([0-9]*\.[0-9]+))) {
    yylval.dval = atof(yytext);
    return NUMBER;
}


^\<.*\> {
	if (strcasecmp(yytext,"<wall") == 0) {
		curtype = WALL;
		return TK_WALL;
	} else if (strcasecmp(yytext,"<gate") == 0) {
		return TK_GATE;
	} else if (strcasecmp(yytext,"<earth") == 0) {
		return TK_EARTH;
	} else if (strcasecmp(yytext,"<web") == 0) {
		return TK_WEB;
	} else if (strcasecmp(yytext,"<board") == 0) {
		return TK_BOARD;
	} else if (strcasecmp(yytext,"<step") == 0) {
		return TK_STEP;
	} else if (strcasecmp(yytext,"<host") == 0) {
		return TK_HOST;
	} else if (strcasecmp(yytext,"<doc") == 0) {
		return TK_DOC;
	} else if (strcasecmp(yytext,"<mirage") == 0) {
		return TK_MIRAGE;
	} else if (strcasecmp(yytext,"<thing") == 0) {
		return TK_THING;
	} else if (strcasecmp(yytext,"</vre") == 0) {
		return TK_END;
	} else {
		fprintf(stderr,"Bad type %s at line %d\n", yytext, yylineno);
		exit(1);
	}
}

"box,size=" { return TK_BOX_SIZE; }
"sphere,radius=" { return TK_SPHER_SIZE; }
"torus,radius=" { return TK_TOR_SIZE; }

"," { return COMMA; }

[^ \t\n,]+ {
	if (strncasecmp(yytext,"xp=",3) == 0) {
		yylval.sval = strdup(yytext+3);
		return TEX_XP;
	} else if (strncasecmp(yytext,"yp=",3) == 0) {
		yylval.sval = strdup(yytext+3);
		return TEX_YP;
	} else if (strncasecmp(yytext,"zp=",3) == 0) {
		yylval.sval = strdup(yytext+3);
		return TEX_ZP;
	} else if (strncasecmp(yytext,"xn=",3) == 0) {
		yylval.sval = strdup(yytext+3);
		return TEX_XN;
	} else if (strncasecmp(yytext,"yn=",3) == 0) {
		yylval.sval = strdup(yytext+3);
		return TEX_YN;
	} else if (strncasecmp(yytext,"zn=",3) == 0) {
		yylval.sval = strdup(yytext+3);
		return TEX_ZN;
	} else if (strncasecmp(yytext,"tx=",3) == 0) {
		yylval.sval = strdup(yytext+3);
		return SPHER_TEX;
	} else if (strncasecmp(yytext,"diffuse=",8) == 0) {
                yylval.dval = atof(yytext+8);
                return TK_DIFFUSE;
        } else if (strncasecmp(yytext,"ambient=",8) == 0) {
                yylval.dval = atof(yytext+8);
                return TK_AMBIENT;
        } else if (strncasecmp(yytext,"specular=",9) == 0) {
                yylval.dval = atof(yytext+9);
                return TK_SPECULAR;
        } else if (strncasecmp(yytext,"emission=",9) == 0) {
                yylval.dval = atof(yytext+9);
                return TK_EMISSION;
        } else if (strncasecmp(yytext,"shininess=",10) == 0) {
                yylval.dval = atof(yytext+10);
                return TK_SHININESS;
        } else if (strncasecmp(yytext,"telnet://",9) == 0) {
                yylval.sval = strdup(yytext+9);
                return TLNT;
        } else if (strncasecmp(yytext,"radius2=",8) == 0) {
                yylval.dval = atof(yytext+8);
                return TK_TOR_SIZE2;
        } else {
    		yylval.sval = strdup(yytext);
    		return STRING;
	}
}

%%

int yywrap(void) {
	return 1;
}
