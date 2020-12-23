%{

#include "vrl.hpp"

extern int lineNumber;
extern int charPos;

extern Fichier *fvrl;

extern void yyerror(char *s);
extern int yylex();
extern char *yytext;

extern "C" {
  extern void free(void *p);
}

%}

%union{
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

%token <lineNo>   STRUCTKEY
%token <lineNo>   LEFTBRACEKEY
%token <lineNo>   RIGHTBRACEKEY
%token <lineNo>   VARKEY
%token <lineNo>   SEMICOLONKEY
%token <lineNo>   COMMAKEY
%token <lineNo>   LEFTBRACKETKEY
%token <lineNo>   RIGHTBRACKETKEY
%token <lineNo>   CONSTRUCTORKEY
%token <lineNo>   NEWKEY
%token <lineNo>   LEFTPARENKEY
%token <lineNo>   RIGHTPARENKEY
%token <lineNo>   EQUALKEY
%token <lineNo>   PERIODKEY
%token <lineNo>   COLONKEY
%token <lineNo>   MAINKEY
%token <lineNo>   FUNCTIONKEY
%token <lineNo>   CALLKEY
%token <lineNo>   RETURNKEY
%token <lineNo>   IFKEY
%token <lineNo>   ELSEKEY
%token <lineNo>   FORKEY
%token <lineNo>   DOTKEY
%token <lineNo>   PLUSKEY
%token <lineNo>   MINUSKEY
%token <lineNo>   MULTKEY
%token <lineNo>   DIVKEY
%token <lineNo>   ABSKEY
%token <lineNo>   SUPKEY
%token <lineNo>   INFKEY
%token <lineNo>   SUPEQUALKEY
%token <lineNo>   INFEQUALKEY
%token <lineNo>   EQUALEQUALKEY
%token <lineNo>   EXCLAMATIONEQUALKEY
%token <lineNo>   EXCLAMATIONKEY
%token <lineNo>   ORKEY
%token <lineNo>   ANDKEY
%token <chaine>   NAME
%token <chaine>   STRING
%token <valeurint>   INTEGER
%token <valeurfloat>   FLOATNUMBER

%type <fichier>				FILE
%type <structure>			STRUCTURE
%type <structure>			STRUCTBODY1
%type <structureheritee>		STRUCTBODY2
%type <fonction>			FUNCTION
%type <pointdentree>			MAIN
%type <constructeur>			CONSTRUCTOR
%type <constructeur>			CONSTRUCTORBODY
%type <instructionliste>		INSTRUCTIONLIST
%type <instruction>			INSTRUCTION
%type <appeldeconstructeur>		CONSTRUCTOR_CALL
%type <declaration>			DECLARATION 
%type <declarationsimple>		SIMPLE_DECLARATION
%type <declarationcomplexe>		COMPLEX_DECLARATION
%type <assignation>			ASSIGNMENT
%type <assignationchaine>		STRING_ASSIGNMENT
%type <assignationexpression>		EXPRESSION_ASSIGNMENT
%type <assignationvariable>		VARIABLE_ASSIGNMENT
%type <assignationnouvelobjet>		NEWOBJECT_ASSIGNMENT
%type <statement>			STATEMENT
%type <boucleif>			IF_STATEMENT
%type <boucleif>			SIMPLE_IF_STATEMENT
%type <boucleif>			IF_ELSE_STATEMENT
%type <bouclefor>			FOR_STATEMENT
%type <expressionliste>			EXPRESSIONLIST
%type <expression>			EXPRESSION
%type <calculableexpression>		CALCULABLE_EXPRESSION
%type <expressionlogique>		LOGICAL_EXPRESSION
%type <nouvelobjet>			NEWOBJECT
%type <variable>			VARIABLE
%type <variablesimple>			SIMPLE_VARIABLE
%type <variablecomplexe>		COMPLEX_VARIABLE
%type <nomdetableau>			ARRAYNAME
%type <nomliste>			NAMELIST
%type <operationunaire>			UNARY_OPERATION
%type <operationbinaire>		BINARY_OPERATION
%type <comparaison>			COMPARISON
%type <operationunairelogique>		LOGICAL_UNARY_OPERATION
%type <operationbinairelogique>		LOGICAL_BINARY_OPERATION
%type <nombre>                          NUMBER

%left PLUSKEY
%left MINUSKEY
%left MULTKEY
%left DIVKEY
%left ANDKEY
%left ORKEY
%left EXCLAMATIONKEY


%%

FILE : EMPTY
		{ $$ = new Fichier() ; fvrl = $$ ; }

	| FILE STRUCTURE
		{ $1->addStructure($2) ; }

	| FILE FUNCTION
		{ $1->addFonction($2) ; }

	| FILE MAIN
		{ $1->addPointDEntree($2) ; }


STRUCTURE : STRUCTKEY NAME LEFTBRACEKEY STRUCTBODY1 RIGHTBRACEKEY
		{ $$ = $4 ; $$->setNom($2) ; }

	| STRUCTKEY NAME COLONKEY NAMELIST LEFTBRACEKEY STRUCTBODY2 RIGHTBRACEKEY
		{ $$ = $6 ; $$->setNom($2) ; $$->setNomListe($4) ; }


STRUCTBODY1 : EMPTY
		{ $$ = new Structure() ; }

	| STRUCTBODY1 SIMPLE_DECLARATION
		{ $1->addDeclaration($2) ; }

	| STRUCTBODY1 FUNCTION
		{ $1->addFonction($2) ; }

	| STRUCTBODY1 CONSTRUCTOR
		{ $1->addConstructeur($2) ; }

STRUCTBODY2 : EMPTY
		{ $$ = new StructureHeritee() ; }

	| STRUCTBODY2 SIMPLE_DECLARATION
		{ $1->addDeclaration($2) ; }

	| STRUCTBODY2 FUNCTION
		{ $1->addFonction($2) ; }

	| STRUCTBODY2 CONSTRUCTOR
		{ $1->addConstructeur($2) ; }

FUNCTION : FUNCTIONKEY NAME LEFTBRACEKEY INSTRUCTIONLIST RIGHTBRACEKEY
		{ $$ = new Fonction($2, $4) ; }

	| FUNCTIONKEY NAME LEFTPARENKEY RIGHTPARENKEY LEFTBRACEKEY INSTRUCTIONLIST RIGHTBRACEKEY
		{ $$ = new Fonction($2, $6) ; }

	| FUNCTIONKEY NAME LEFTPARENKEY NAMELIST RIGHTPARENKEY LEFTBRACEKEY INSTRUCTIONLIST RIGHTBRACEKEY
		{ $$ = new Fonction($2, $4, $7) ; }


MAIN : MAINKEY LEFTPARENKEY RIGHTPARENKEY LEFTBRACEKEY INSTRUCTIONLIST RIGHTBRACEKEY
		{ $$ = new PointDEntree($5) ; }


CONSTRUCTOR : CONSTRUCTORKEY LEFTPARENKEY RIGHTPARENKEY LEFTBRACEKEY CONSTRUCTORBODY RIGHTBRACEKEY
		{ $$ = $5 ; }

	| CONSTRUCTORKEY LEFTPARENKEY NAMELIST RIGHTPARENKEY LEFTBRACEKEY CONSTRUCTORBODY RIGHTBRACEKEY
		{ $$ = $6 ; $$->setNomListe($3) ; }


CONSTRUCTORBODY : EMPTY
		{ $$ = new Constructeur() ; }

	| CONSTRUCTORBODY CONSTRUCTOR_CALL
		{ $1->addAppelDeConstructeur($2) ; }

	| CONSTRUCTORBODY INSTRUCTION
		{ $1->addInstruction($2) ; }



INSTRUCTIONLIST : EMPTY
		{ $$ = new InstructionListe() ; }

	| INSTRUCTIONLIST INSTRUCTION
		{ $1->addInstruction($2) ; }


INSTRUCTION : DECLARATION
		{ $$ = $1 ; }

	| ASSIGNMENT
		{ ; $$ = $1 ; }

	| STATEMENT
		{ $$ = $1 ; }


CONSTRUCTOR_CALL : 	NAME LEFTPARENKEY  RIGHTPARENKEY SEMICOLONKEY
		{ $$ = new AppelDeConstructeur($1) ; }		

	| NAME LEFTPARENKEY EXPRESSIONLIST RIGHTPARENKEY SEMICOLONKEY
		{ $$ = new AppelDeConstructeur($1, $3) ; }


DECLARATION : SIMPLE_DECLARATION
		{ $$ = $1 ; }

	| COMPLEX_DECLARATION
		{ $$ = $1 ; }


ASSIGNMENT : STRING_ASSIGNMENT
		{ $$ = $1 ; }

	| EXPRESSION_ASSIGNMENT
		{ $$ = $1 ; }

	| VARIABLE_ASSIGNMENT
		{ $$ = $1 ; }

	| NEWOBJECT_ASSIGNMENT
		{ $$ = $1 ; }


STATEMENT : IF_STATEMENT
		{ $$ = $1 ; }

	| FOR_STATEMENT
		{ $$ = $1 ; }

	| RETURNKEY EXPRESSION SEMICOLONKEY
		{ $$ = new Retour($2) ; }


SIMPLE_DECLARATION : VARKEY NAMELIST SEMICOLONKEY
		{ $$ = new DeclarationSimple($2) ; }

	| VARKEY ARRAYNAME SEMICOLONKEY
		{ $$ = new DeclarationSimple($2) ; }


COMPLEX_DECLARATION : VARKEY VARIABLE EQUALKEY STRING SEMICOLONKEY
		{ $$ = new DeclarationComplexeChaine($2, $4) ; }

	| VARKEY VARIABLE EQUALKEY EXPRESSION SEMICOLONKEY
		{ $$ = new DeclarationComplexeExpression($2, $4) ; }
/*
	| VARKEY VARIABLE EQUALKEY NEWOBJECT SEMICOLONKEY
		{ $$ = new DeclarationComplexeNouvelObjet($2, $4) ; }
non implementee */

STRING_ASSIGNMENT : VARIABLE EQUALKEY STRING SEMICOLONKEY
		{ $$ = new AssignationChaine($1,$3) ; }


EXPRESSION_ASSIGNMENT : VARIABLE EQUALKEY CALCULABLE_EXPRESSION SEMICOLONKEY
		{ $$ = new AssignationExpression($1,$3) ; }


VARIABLE_ASSIGNMENT : VARIABLE EQUALKEY VARIABLE SEMICOLONKEY
		{ $$ = new AssignationVariable($1, $3); }



NEWOBJECT_ASSIGNMENT : VARIABLE EQUALKEY NEWOBJECT SEMICOLONKEY
		{ $$ = new AssignationNouvelObjet($1,$3) ; }


IF_STATEMENT : SIMPLE_IF_STATEMENT
		{ $$ = $1 ; }

	| IF_ELSE_STATEMENT
		{ $$ = $1 ; }


SIMPLE_IF_STATEMENT : IFKEY LOGICAL_EXPRESSION LEFTBRACEKEY INSTRUCTIONLIST RIGHTBRACEKEY
		{ $$ = new BoucleIf($2,$4) ; }


IF_ELSE_STATEMENT : IFKEY LOGICAL_EXPRESSION LEFTBRACEKEY INSTRUCTIONLIST RIGHTBRACEKEY ELSEKEY LEFTBRACEKEY INSTRUCTIONLIST RIGHTBRACEKEY
		{ $$ = new BoucleIf($2,$4,$8) ; }


FOR_STATEMENT : FORKEY LEFTPARENKEY EXPRESSION_ASSIGNMENT LOGICAL_EXPRESSION SEMICOLONKEY EXPRESSION_ASSIGNMENT RIGHTPARENKEY LEFTBRACEKEY INSTRUCTIONLIST RIGHTBRACEKEY
		{ $$ = new BoucleFor($3, $4, $6, $9) ; }


EXPRESSIONLIST :  EXPRESSION
		{ $$ = new ExpressionListe($1) ; }

	| EXPRESSIONLIST COMMAKEY EXPRESSION
		{ $1->addExpression($3) ; }

	

EXPRESSION : CALCULABLE_EXPRESSION
		{ $$ = $1 ; }
	
	| VARIABLE
		{ $$ = $1 ; }


CALCULABLE_EXPRESSION : NUMBER
		{ $$ = $1 ; }

	|  CALLKEY NAME LEFTPARENKEY  RIGHTPARENKEY
		{ $$ = new AppelDeFonction($2) ; }

	|  CALLKEY NAME LEFTPARENKEY EXPRESSIONLIST RIGHTPARENKEY
		{ $$ = new AppelDeFonction($2, $4) ; }

	| BINARY_OPERATION
		{ $$ = $1 ; }

	| UNARY_OPERATION
		{ $$ = $1 ; }

	| LEFTPARENKEY EXPRESSION RIGHTPARENKEY
		{ $$ = new ExpressionCalculable($2) ; }


LOGICAL_EXPRESSION : COMPARISON
		{ $$ = $1 ; }

	| LOGICAL_BINARY_OPERATION
		{ $$ = $1 ; }

	| LOGICAL_UNARY_OPERATION
		{ $$ = $1 ; }

	| LEFTPARENKEY LOGICAL_EXPRESSION RIGHTPARENKEY
		{ $$ = $2 ; }


NEWOBJECT :  NEWKEY NAME LEFTPARENKEY  RIGHTPARENKEY
		{/* erreur ici */ $$ = new NouvelObjet($2) ; }
	
	|NEWKEY NAME LEFTPARENKEY EXPRESSIONLIST RIGHTPARENKEY
		{/* erreur ici */ $$ = new NouvelObjet($2, $4) ; }




VARIABLE : SIMPLE_VARIABLE
		{ $$ = $1 ; }

	| COMPLEX_VARIABLE
		{ $$ = $1 ; }

SIMPLE_VARIABLE : NAME	
		{ $$ = new Nom($1) ; }

	| ARRAYNAME
		{ $$ = $1 ; }

ARRAYNAME : /*NAME LEFTBRACKETKEY INTEGER RIGHTBRACKETKEY
		{ $$ = new NomDeTableau($1, $3) ; }

	|*/ NAME LEFTBRACKETKEY EXPRESSION RIGHTBRACKETKEY
		{ $$ = new NomDeTableau($1, $3) ; }



COMPLEX_VARIABLE : SIMPLE_VARIABLE DOTKEY SIMPLE_VARIABLE
		 { $$ = new VariableComplexe($1, $3) ; }

	| COMPLEX_VARIABLE DOTKEY SIMPLE_VARIABLE
		{ $1->addNomComposant($3) ; }


NAMELIST : NAME
		{ $$ = new NomListe($1) ; } 

	| NAMELIST COMMAKEY NAME
		{ $1->addNom($3) ; }


UNARY_OPERATION : LEFTPARENKEY MINUSKEY EXPRESSION RIGHTPARENKEY
		{ $$ = new Negation($3) ; }

	| ABSKEY LEFTPARENKEY EXPRESSION RIGHTPARENKEY 
		{ $$ = new Absolu($3) ; }


BINARY_OPERATION : EXPRESSION PLUSKEY EXPRESSION
		{ $$ = new Addition($1, $3) ; }

	| EXPRESSION MINUSKEY EXPRESSION
		{ $$ = new Soustraction($1, $3) ; }

	| EXPRESSION MULTKEY EXPRESSION
		{ $$ = new Multiplication($1, $3) ; }

	| EXPRESSION DIVKEY EXPRESSION
		{ $$ = new Division($1, $3) ; }


COMPARISON : EXPRESSION SUPKEY EXPRESSION
		{ $$ = new Superieur($1, $3) ; }

	| EXPRESSION INFKEY EXPRESSION
		{ $$ = new Inferieur($1, $3) ; }

	| EXPRESSION SUPEQUALKEY EXPRESSION
		{ $$ = new SuperieurOuEgal($1, $3) ; }

	| EXPRESSION INFEQUALKEY EXPRESSION
		{ $$ = new InferieurOuEgal($1, $3) ; }

	| EXPRESSION EQUALEQUALKEY EXPRESSION
		{ $$ = new EgalEgal($1, $3) ; }

	| EXPRESSION EXCLAMATIONEQUALKEY EXPRESSION
		{ $$ = new Different($1, $3) ; }


LOGICAL_UNARY_OPERATION : EXCLAMATIONKEY LOGICAL_EXPRESSION
		{ $$ = new NegationLogique($2) ; }


LOGICAL_BINARY_OPERATION : LOGICAL_EXPRESSION ORKEY LOGICAL_EXPRESSION
		{ $$ = new OuLogique($1, $3) ; }

	| LOGICAL_EXPRESSION ANDKEY LOGICAL_EXPRESSION
		{ $$ = new EtLogique($1, $3) ; }


NUMBER : INTEGER
		{ $$ = new Nombre($1) ; }
	| FLOATNUMBER
		{ $$ = new Nombre($1) ; }


EMPTY : {}

%%

void yyerror(char *s) {
	fprintf(stderr, "### Error : %s\n", s) ;
	fprintf(stderr, "### Line=%d, Char=%dth : ''%s''\n", lineNumber, charPos, yytext) ;
	exit(-1) ;
} ;
