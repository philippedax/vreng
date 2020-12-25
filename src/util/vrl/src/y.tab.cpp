/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 1 "vrl.y"


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



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

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
	DeclarationComplexeExpr        *declarationcomplexeexpr ;
	DeclarationComplexeNouvelObjet *declarationcomplexenouvelobjet ;
	Assign                         *assign ;
	AssignChaine	               *assignchaine ;
	AssignExpr	               *assignexpr ;
	AssignVariable	               *assignvariable ;
	AssignNouvelObjet              *assignnouvelobjet ;
	NouvelObjet                    *nouvelobjet ;
	ExprListe                      *exprliste ;
	Expr                           *expr ;
	ExprCalculable	               *calculableexpr ;
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
	ExprLogiqueListe               *exprlogiqueliste ;
	ExprLogique                    *exprlogique ;
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
/* Line 193 of yacc.c.  */
#line 278 "y.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 291 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   340

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  44
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  42
/* YYNRULES -- Number of rules.  */
#define YYNRULES  96
/* YYNRULES -- Number of states.  */
#define YYNSTATES  212

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   298

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    11,    14,    20,    28,    30,
      33,    36,    39,    41,    44,    47,    50,    56,    64,    73,
      80,    87,    95,    97,   100,   103,   105,   108,   110,   112,
     114,   119,   125,   127,   129,   131,   133,   135,   137,   139,
     141,   145,   149,   153,   159,   165,   170,   175,   180,   185,
     187,   189,   195,   205,   216,   218,   222,   224,   226,   228,
     233,   239,   241,   243,   247,   249,   251,   253,   257,   262,
     268,   270,   272,   274,   276,   281,   285,   289,   291,   295,
     300,   305,   309,   313,   317,   321,   325,   329,   333,   337,
     341,   345,   348,   352,   356,   358,   360
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      45,     0,    -1,    85,    -1,    45,    46,    -1,    45,    49,
      -1,    45,    50,    -1,     3,    40,     4,    47,     5,    -1,
       3,    40,    17,    78,     4,    48,     5,    -1,    85,    -1,
      47,    59,    -1,    47,    49,    -1,    47,    51,    -1,    85,
      -1,    48,    59,    -1,    48,    49,    -1,    48,    51,    -1,
      19,    40,     4,    53,     5,    -1,    19,    40,    13,    14,
       4,    53,     5,    -1,    19,    40,    13,    78,    14,     4,
      53,     5,    -1,    18,    13,    14,     4,    53,     5,    -1,
      11,    13,    14,     4,    52,     5,    -1,    11,    13,    78,
      14,     4,    52,     5,    -1,    85,    -1,    52,    55,    -1,
      52,    54,    -1,    85,    -1,    53,    54,    -1,    56,    -1,
      57,    -1,    58,    -1,    40,    13,    14,     7,    -1,    40,
      13,    69,    14,     7,    -1,    59,    -1,    60,    -1,    61,
      -1,    62,    -1,    63,    -1,    64,    -1,    65,    -1,    68,
      -1,    21,    70,     7,    -1,     6,    78,     7,    -1,     6,
      76,     7,    -1,     6,    74,    15,    41,     7,    -1,     6,
      74,    15,    70,     7,    -1,    74,    15,    41,     7,    -1,
      74,    15,    71,     7,    -1,    74,    15,    74,     7,    -1,
      74,    15,    73,     7,    -1,    66,    -1,    67,    -1,    22,
      72,     4,    53,     5,    -1,    22,    72,     4,    53,     5,
      23,     4,    53,     5,    -1,    24,    13,    62,    72,     7,
      62,    14,     4,    53,     5,    -1,    70,    -1,    69,     8,
      70,    -1,    71,    -1,    74,    -1,    84,    -1,    20,    40,
      13,    14,    -1,    20,    40,    13,    69,    14,    -1,    80,
      -1,    79,    -1,    13,    70,    14,    -1,    81,    -1,    83,
      -1,    82,    -1,    13,    72,    14,    -1,    12,    40,    13,
      14,    -1,    12,    40,    13,    69,    14,    -1,    75,    -1,
      77,    -1,    40,    -1,    76,    -1,    40,     9,    70,    10,
      -1,    75,    25,    75,    -1,    77,    25,    75,    -1,    40,
      -1,    78,     8,    40,    -1,    13,    27,    70,    14,    -1,
      30,    13,    70,    14,    -1,    70,    26,    70,    -1,    70,
      27,    70,    -1,    70,    28,    70,    -1,    70,    29,    70,
      -1,    70,    31,    70,    -1,    70,    32,    70,    -1,    70,
      33,    70,    -1,    70,    34,    70,    -1,    70,    35,    70,
      -1,    70,    36,    70,    -1,    37,    72,    -1,    72,    38,
      72,    -1,    72,    39,    72,    -1,    42,    -1,    43,    -1,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   191,   191,   193,   195,   197,   200,   202,   205,   207,
     209,   211,   214,   216,   218,   220,   223,   225,   227,   230,
     233,   235,   238,   240,   242,   245,   247,   250,   252,   254,
     257,   259,   262,   264,   267,   269,   271,   273,   276,   278,
     280,   283,   285,   288,   290,   297,   300,   303,   306,   309,
     311,   314,   317,   320,   323,   325,   328,   330,   333,   335,
     337,   339,   341,   343,   346,   348,   350,   352,   355,   357,
     360,   362,   365,   367,   372,   375,   377,   380,   382,   385,
     387,   390,   392,   394,   396,   399,   401,   403,   405,   407,
     409,   412,   415,   417,   420,   422,   426
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "STRUCTKEY", "LEFTBRACEKEY",
  "RIGHTBRACEKEY", "VARKEY", "SEMICOLONKEY", "COMMAKEY", "LEFTBRACKETKEY",
  "RIGHTBRACKETKEY", "CONSTRUCTORKEY", "NEWKEY", "LEFTPARENKEY",
  "RIGHTPARENKEY", "EQUALKEY", "PERIODKEY", "COLONKEY", "MAINKEY",
  "FUNCTIONKEY", "CALLKEY", "RETURNKEY", "IFKEY", "ELSEKEY", "FORKEY",
  "DOTKEY", "PLUSKEY", "MINUSKEY", "MULTKEY", "DIVKEY", "ABSKEY", "SUPKEY",
  "INFKEY", "SUPEQUALKEY", "INFEQUALKEY", "EQUALEQUALKEY",
  "EXCLAMATIONEQUALKEY", "EXCLAMATIONKEY", "ORKEY", "ANDKEY", "NAME",
  "STRING", "INTEGER", "FLOATNUMBER", "$accept", "FILE", "STRUCTURE",
  "STRUCTBODY1", "STRUCTBODY2", "FUNCTION", "MAIN", "CONSTRUCTOR",
  "CONSTRUCTORBODY", "INSTRUCTIONLIST", "INSTRUCTION", "CONSTRUCTOR_CALL",
  "DECLARATION", "ASSIGNMENT", "STATEMENT", "SIMPLE_DECLARATION",
  "COMPLEX_DECLARATION", "STRING_ASSIGNMENT", "EXPRESSION_ASSIGNMENT",
  "VARIABLE_ASSIGNMENT", "NEWOBJECT_ASSIGNMENT", "IF_STATEMENT",
  "SIMPLE_IF_STATEMENT", "IF_ELSE_STATEMENT", "FOR_STATEMENT",
  "EXPRESSIONLIST", "EXPRESSION", "CALCULABLE_EXPRESSION",
  "LOGICAL_EXPRESSION", "NEWOBJECT", "VARIABLE", "SIMPLE_VARIABLE",
  "ARRAYNAME", "COMPLEX_VARIABLE", "NAMELIST", "UNARY_OPERATION",
  "BINARY_OPERATION", "COMPARISON", "LOGICAL_UNARY_OPERATION",
  "LOGICAL_BINARY_OPERATION", "NUMBER", "EMPTY", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    44,    45,    45,    45,    45,    46,    46,    47,    47,
      47,    47,    48,    48,    48,    48,    49,    49,    49,    50,
      51,    51,    52,    52,    52,    53,    53,    54,    54,    54,
      55,    55,    56,    56,    57,    57,    57,    57,    58,    58,
      58,    59,    59,    60,    60,    61,    62,    63,    64,    65,
      65,    66,    67,    68,    69,    69,    70,    70,    71,    71,
      71,    71,    71,    71,    72,    72,    72,    72,    73,    73,
      74,    74,    75,    75,    76,    77,    77,    78,    78,    79,
      79,    80,    80,    80,    80,    81,    81,    81,    81,    81,
      81,    82,    83,    83,    84,    84,    85
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     2,     2,     5,     7,     1,     2,
       2,     2,     1,     2,     2,     2,     5,     7,     8,     6,
       6,     7,     1,     2,     2,     1,     2,     1,     1,     1,
       4,     5,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     3,     5,     5,     4,     4,     4,     4,     1,
       1,     5,     9,    10,     1,     3,     1,     1,     1,     4,
       5,     1,     1,     3,     1,     1,     1,     3,     4,     5,
       1,     1,     1,     1,     4,     3,     3,     1,     3,     4,
       4,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     3,     3,     1,     1,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
      96,     0,     2,     1,     0,     0,     0,     3,     4,     5,
       0,     0,     0,    96,     0,     0,    96,     0,     0,     8,
      77,     0,    96,     0,    25,     0,     0,     6,     0,     0,
      10,    11,     9,    96,     0,     0,    16,     0,     0,     0,
       0,    72,    26,    27,    28,    29,    32,    33,    34,    35,
      36,    37,    38,    49,    50,    39,     0,    70,    73,    71,
      96,     0,    77,     0,     0,     0,     0,    12,    78,    19,
      72,     0,    73,     0,     0,     0,    94,    95,     0,    56,
      57,    62,    61,    58,     0,     0,     0,     0,    64,    66,
      65,     0,     0,     0,     0,     0,     0,    96,    42,    41,
       0,     0,     7,    14,    15,    13,     0,     0,     0,     0,
       0,    40,     0,     0,     0,     0,     0,     0,    91,     0,
       0,     0,     0,     0,     0,    96,     0,     0,     0,     0,
       0,     0,     0,     0,    56,     0,    57,    75,    76,    17,
       0,    96,     0,     0,     0,     0,    63,     0,     0,    81,
      82,    83,    84,    67,    85,    86,    87,    88,    89,    90,
       0,    92,    93,     0,     0,    74,     0,    45,    46,    48,
      47,    18,     0,    22,    96,    43,    44,    79,    59,     0,
      54,    80,    51,     0,     0,    20,    72,    24,    23,     0,
       0,    60,     0,     0,    68,     0,     0,    21,    55,    96,
       0,    69,     0,     0,     0,    96,    30,     0,    52,     0,
      31,    53
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     7,    18,    66,     8,     9,    31,   172,    23,
      42,   188,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,   179,    86,    79,    87,   135,
      80,    57,    58,    59,    64,    81,    82,    88,    89,    90,
      83,    24
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -157
static const yytype_int16 yypact[] =
{
    -157,   111,  -157,  -157,     8,    43,    37,  -157,  -157,  -157,
      15,    70,    21,  -157,    78,   161,  -157,     3,    17,  -157,
    -157,   123,  -157,     5,  -157,   169,    94,  -157,   101,   162,
    -157,  -157,  -157,  -157,   142,    25,  -157,   143,   236,   196,
     164,   176,  -157,  -157,  -157,  -157,  -157,  -157,  -157,  -157,
    -157,  -157,  -157,  -157,  -157,  -157,   190,   172,  -157,   177,
    -157,   185,   176,   203,   154,     4,   128,  -157,  -157,  -157,
     184,   200,   203,   221,   195,   224,  -157,  -157,   141,  -157,
    -157,  -157,  -157,  -157,   181,   196,   296,    20,  -157,  -157,
    -157,   217,   236,   187,   217,   217,    45,  -157,  -157,  -157,
     227,   136,  -157,  -157,  -157,  -157,   212,   236,    72,   254,
     236,  -157,   236,   236,   236,   236,   263,    -1,  -157,   236,
     236,   236,   236,   236,   236,  -157,   196,   196,   196,   225,
     276,   228,   262,   281,   264,   267,   273,  -157,  -157,  -157,
      88,  -157,   289,   294,   255,   286,  -157,    83,   292,   218,
     191,   282,  -157,  -157,   281,   281,   281,   281,   281,   281,
     100,  -157,   278,    33,   236,  -157,   304,  -157,  -157,  -157,
    -157,  -157,   114,  -157,  -157,  -157,  -157,  -157,  -157,   137,
     281,  -157,   303,   217,   230,  -157,   133,  -157,  -157,   150,
     236,  -157,   329,   320,  -157,   145,   245,  -157,   281,  -157,
     331,  -157,   330,   149,   174,  -157,  -157,   332,  -157,   182,
    -157,  -157
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -157,  -157,  -157,  -157,  -157,   -13,  -157,   270,   166,   -21,
    -156,  -157,  -157,  -157,  -157,    -9,  -157,  -157,   -88,  -157,
    -157,  -157,  -157,  -157,  -157,  -142,   -32,   -85,   -64,  -157,
     -23,   118,    79,  -157,   -10,  -157,  -157,  -157,  -157,  -157,
    -157,     2
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -78
static const yytype_int16 yytable[] =
{
      56,    35,     2,   128,    21,    30,    78,    26,   134,    32,
      36,    37,    56,   153,    71,    19,   187,    25,   100,    13,
     117,   118,    27,    28,   125,    16,    38,    39,    29,    40,
      69,    37,    14,   187,    17,    67,     6,   126,   127,    96,
     183,   108,   195,    20,    20,    41,    38,    39,    10,    40,
     139,    37,   116,   103,   203,   101,    11,   105,   126,   127,
     130,   133,   161,   162,   163,    41,    38,    39,   129,    40,
     136,   126,   127,    56,   144,   145,   140,    12,   148,   134,
     149,   150,   151,   152,    15,    41,   146,   154,   155,   156,
     157,   158,   159,   171,    37,   193,    73,   178,   112,   113,
     114,   115,    34,    74,   160,   182,    37,    63,    61,    38,
      39,     3,    40,    75,     4,   180,    72,    56,    20,   185,
      37,    38,    39,    41,    40,    76,    77,    33,    41,     5,
       6,    34,   133,   102,    28,    38,    39,    56,    40,    29,
      41,    62,    92,   173,    34,   190,   196,     6,   111,    56,
     142,   191,   180,   190,   186,   197,    37,   190,   198,   201,
     129,    99,    34,   207,   180,    22,    56,   112,   113,   114,
     115,    38,    39,    60,    40,    65,   173,    91,   204,   208,
      37,    56,    68,    70,   209,    92,    56,   211,    37,    97,
     186,   -77,   -77,    92,    84,    38,    39,    94,    40,   131,
      73,    74,    95,    38,    39,    93,    40,    74,   107,    84,
      98,    75,   137,   138,    41,   106,    74,    75,    85,   114,
     115,    41,    41,    76,    77,    73,    75,    41,   132,    76,
      77,   141,    74,    85,    73,   109,    41,   110,    76,    77,
     164,    74,    75,    73,   194,   113,   114,   115,   107,    73,
      74,    75,    41,   143,    76,    77,    74,    41,    73,   202,
      75,    41,   176,    76,    77,    74,    75,   147,   166,   167,
      41,   168,    76,    77,   169,    75,    41,   146,    76,    77,
     170,   112,   113,   114,   115,    41,   165,    76,    77,   112,
     113,   114,   115,   174,   119,   120,   121,   122,   123,   124,
     177,   175,   112,   113,   114,   115,   181,   112,   113,   114,
     115,   115,   112,   113,   114,   115,   126,   184,   112,   113,
     114,   115,   112,   113,   114,   115,   192,   119,   120,   121,
     122,   123,   124,   199,   200,   205,   104,   206,     0,   210,
     189
};

static const yytype_int16 yycheck[] =
{
      23,    22,     0,    91,    14,    18,    38,    17,    93,    18,
       5,     6,    35,    14,    37,    13,   172,    14,    14,     4,
      84,    85,     5,     6,     4,     4,    21,    22,    11,    24,
       5,     6,    17,   189,    13,    33,    19,    38,    39,    60,
       7,    73,   184,    40,    40,    40,    21,    22,    40,    24,
       5,     6,    84,    66,   196,    65,    13,    66,    38,    39,
      92,    93,   126,   127,   128,    40,    21,    22,    91,    24,
      93,    38,    39,    96,   106,   107,    97,    40,   110,   164,
     112,   113,   114,   115,    14,    40,    14,   119,   120,   121,
     122,   123,   124,     5,     6,   183,    13,    14,    26,    27,
      28,    29,     8,    20,   125,     5,     6,    28,    14,    21,
      22,     0,    24,    30,     3,   147,    37,   140,    40,     5,
       6,    21,    22,    40,    24,    42,    43,     4,    40,    18,
      19,     8,   164,     5,     6,    21,    22,   160,    24,    11,
      40,    40,     9,   141,     8,     8,    13,    19,     7,   172,
      14,    14,   184,     8,    40,     5,     6,     8,   190,    14,
     183,     7,     8,    14,   196,     4,   189,    26,    27,    28,
      29,    21,    22,     4,    24,    13,   174,    13,   199,     5,
       6,   204,    40,    40,   205,     9,   209,     5,     6,     4,
      40,     7,     8,     9,    13,    21,    22,    25,    24,    12,
      13,    20,    25,    21,    22,    15,    24,    20,    27,    13,
       7,    30,    94,    95,    40,    15,    20,    30,    37,    28,
      29,    40,    40,    42,    43,    13,    30,    40,    41,    42,
      43,     4,    20,    37,    13,    40,    40,    13,    42,    43,
      15,    20,    30,    13,    14,    27,    28,    29,    27,    13,
      20,    30,    40,    41,    42,    43,    20,    40,    13,    14,
      30,    40,     7,    42,    43,    20,    30,    13,    40,     7,
      40,     7,    42,    43,     7,    30,    40,    14,    42,    43,
       7,    26,    27,    28,    29,    40,    10,    42,    43,    26,
      27,    28,    29,     4,    31,    32,    33,    34,    35,    36,
      14,     7,    26,    27,    28,    29,    14,    26,    27,    28,
      29,    29,    26,    27,    28,    29,    38,    13,    26,    27,
      28,    29,    26,    27,    28,    29,    23,    31,    32,    33,
      34,    35,    36,     4,    14,     4,    66,     7,    -1,     7,
     174
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    45,    85,     0,     3,    18,    19,    46,    49,    50,
      40,    13,    40,     4,    17,    14,     4,    13,    47,    85,
      40,    78,     4,    53,    85,    14,    78,     5,     6,    11,
      49,    51,    59,     4,     8,    53,     5,     6,    21,    22,
      24,    40,    54,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    74,    75,    76,    77,
       4,    14,    40,    76,    78,    13,    48,    85,    40,     5,
      40,    74,    76,    13,    20,    30,    42,    43,    70,    71,
      74,    79,    80,    84,    13,    37,    70,    72,    81,    82,
      83,    13,     9,    15,    25,    25,    53,     4,     7,     7,
      14,    78,     5,    49,    51,    59,    15,    27,    70,    40,
      13,     7,    26,    27,    28,    29,    70,    72,    72,    31,
      32,    33,    34,    35,    36,     4,    38,    39,    62,    74,
      70,    12,    41,    70,    71,    73,    74,    75,    75,     5,
      53,     4,    14,    41,    70,    70,    14,    13,    70,    70,
      70,    70,    70,    14,    70,    70,    70,    70,    70,    70,
      53,    72,    72,    72,    15,    10,    40,     7,     7,     7,
       7,     5,    52,    85,     4,     7,     7,    14,    14,    69,
      70,    14,     5,     7,    13,     5,    40,    54,    55,    52,
       8,    14,    23,    62,    14,    69,    13,     5,    70,     4,
      14,    14,    14,    69,    53,     4,     7,    14,     5,    53,
       7,     5
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 192 "vrl.y"
    { (yyval.fichier) = new Fichier() ; fvrl = (yyval.fichier) ; }
    break;

  case 3:
#line 194 "vrl.y"
    { (yyvsp[(1) - (2)].fichier)->addStructure((yyvsp[(2) - (2)].structure)) ; }
    break;

  case 4:
#line 196 "vrl.y"
    { (yyvsp[(1) - (2)].fichier)->addFonction((yyvsp[(2) - (2)].fonction)) ; }
    break;

  case 5:
#line 198 "vrl.y"
    { (yyvsp[(1) - (2)].fichier)->addPointDEntree((yyvsp[(2) - (2)].pointdentree)) ; }
    break;

  case 6:
#line 201 "vrl.y"
    { (yyval.structure) = (yyvsp[(4) - (5)].structure) ; (yyval.structure)->setNom((yyvsp[(2) - (5)].chaine)) ; }
    break;

  case 7:
#line 203 "vrl.y"
    { (yyval.structure) = (yyvsp[(6) - (7)].structureheritee) ; (yyval.structure)->setNom((yyvsp[(2) - (7)].chaine)) ; (yyval.structure)->setNomListe((yyvsp[(4) - (7)].nomliste)) ; }
    break;

  case 8:
#line 206 "vrl.y"
    { (yyval.structure) = new Structure() ; }
    break;

  case 9:
#line 208 "vrl.y"
    { (yyvsp[(1) - (2)].structure)->addDeclaration((yyvsp[(2) - (2)].declarationsimple)) ; }
    break;

  case 10:
#line 210 "vrl.y"
    { (yyvsp[(1) - (2)].structure)->addFonction((yyvsp[(2) - (2)].fonction)) ; }
    break;

  case 11:
#line 212 "vrl.y"
    { (yyvsp[(1) - (2)].structure)->addConstructeur((yyvsp[(2) - (2)].constructeur)) ; }
    break;

  case 12:
#line 215 "vrl.y"
    { (yyval.structureheritee) = new StructureHeritee() ; }
    break;

  case 13:
#line 217 "vrl.y"
    { (yyvsp[(1) - (2)].structureheritee)->addDeclaration((yyvsp[(2) - (2)].declarationsimple)) ; }
    break;

  case 14:
#line 219 "vrl.y"
    { (yyvsp[(1) - (2)].structureheritee)->addFonction((yyvsp[(2) - (2)].fonction)) ; }
    break;

  case 15:
#line 221 "vrl.y"
    { (yyvsp[(1) - (2)].structureheritee)->addConstructeur((yyvsp[(2) - (2)].constructeur)) ; }
    break;

  case 16:
#line 224 "vrl.y"
    { (yyval.fonction) = new Fonction((yyvsp[(2) - (5)].chaine), (yyvsp[(4) - (5)].instructionliste)) ; }
    break;

  case 17:
#line 226 "vrl.y"
    { (yyval.fonction) = new Fonction((yyvsp[(2) - (7)].chaine), (yyvsp[(6) - (7)].instructionliste)) ; }
    break;

  case 18:
#line 228 "vrl.y"
    { (yyval.fonction) = new Fonction((yyvsp[(2) - (8)].chaine), (yyvsp[(4) - (8)].nomliste), (yyvsp[(7) - (8)].instructionliste)) ; }
    break;

  case 19:
#line 231 "vrl.y"
    { (yyval.pointdentree) = new PointDEntree((yyvsp[(5) - (6)].instructionliste)) ; }
    break;

  case 20:
#line 234 "vrl.y"
    { (yyval.constructeur) = (yyvsp[(5) - (6)].constructeur) ; }
    break;

  case 21:
#line 236 "vrl.y"
    { (yyval.constructeur) = (yyvsp[(6) - (7)].constructeur) ; (yyval.constructeur)->setNomListe((yyvsp[(3) - (7)].nomliste)) ; }
    break;

  case 22:
#line 239 "vrl.y"
    { (yyval.constructeur) = new Constructeur() ; }
    break;

  case 23:
#line 241 "vrl.y"
    { (yyvsp[(1) - (2)].constructeur)->addAppelDeConstructeur((yyvsp[(2) - (2)].appeldeconstructeur)) ; }
    break;

  case 24:
#line 243 "vrl.y"
    { (yyvsp[(1) - (2)].constructeur)->addInstruction((yyvsp[(2) - (2)].instruction)) ; }
    break;

  case 25:
#line 246 "vrl.y"
    { (yyval.instructionliste) = new InstructionListe() ; }
    break;

  case 26:
#line 248 "vrl.y"
    { (yyvsp[(1) - (2)].instructionliste)->addInstruction((yyvsp[(2) - (2)].instruction)) ; }
    break;

  case 27:
#line 251 "vrl.y"
    { (yyval.instruction) = (yyvsp[(1) - (1)].declaration) ; }
    break;

  case 28:
#line 253 "vrl.y"
    { ; (yyval.instruction) = (yyvsp[(1) - (1)].assign) ; }
    break;

  case 29:
#line 255 "vrl.y"
    { (yyval.instruction) = (yyvsp[(1) - (1)].statement) ; }
    break;

  case 30:
#line 258 "vrl.y"
    { (yyval.appeldeconstructeur) = new AppelDeConstructeur((yyvsp[(1) - (4)].chaine)) ; }
    break;

  case 31:
#line 260 "vrl.y"
    { (yyval.appeldeconstructeur) = new AppelDeConstructeur((yyvsp[(1) - (5)].chaine), (yyvsp[(3) - (5)].exprliste)) ; }
    break;

  case 32:
#line 263 "vrl.y"
    { (yyval.declaration) = (yyvsp[(1) - (1)].declarationsimple) ; }
    break;

  case 33:
#line 265 "vrl.y"
    { (yyval.declaration) = (yyvsp[(1) - (1)].declarationcomplexe) ; }
    break;

  case 34:
#line 268 "vrl.y"
    { (yyval.assign) = (yyvsp[(1) - (1)].assignchaine) ; }
    break;

  case 35:
#line 270 "vrl.y"
    { (yyval.assign) = (yyvsp[(1) - (1)].assignexpr) ; }
    break;

  case 36:
#line 272 "vrl.y"
    { (yyval.assign) = (yyvsp[(1) - (1)].assignvariable) ; }
    break;

  case 37:
#line 274 "vrl.y"
    { (yyval.assign) = (yyvsp[(1) - (1)].assignnouvelobjet) ; }
    break;

  case 38:
#line 277 "vrl.y"
    { (yyval.statement) = (yyvsp[(1) - (1)].boucleif) ; }
    break;

  case 39:
#line 279 "vrl.y"
    { (yyval.statement) = (yyvsp[(1) - (1)].bouclefor) ; }
    break;

  case 40:
#line 281 "vrl.y"
    { (yyval.statement) = new Retour((yyvsp[(2) - (3)].expr)) ; }
    break;

  case 41:
#line 284 "vrl.y"
    { (yyval.declarationsimple) = new DeclarationSimple((yyvsp[(2) - (3)].nomliste)) ; }
    break;

  case 42:
#line 286 "vrl.y"
    { (yyval.declarationsimple) = new DeclarationSimple((yyvsp[(2) - (3)].nomdetableau)) ; }
    break;

  case 43:
#line 289 "vrl.y"
    { (yyval.declarationcomplexe) = new DeclarationComplexeChaine((yyvsp[(2) - (5)].variable), (yyvsp[(4) - (5)].chaine)) ; }
    break;

  case 44:
#line 291 "vrl.y"
    { (yyval.declarationcomplexe) = new DeclarationComplexeExpr((yyvsp[(2) - (5)].variable), (yyvsp[(4) - (5)].expr)) ; }
    break;

  case 45:
#line 298 "vrl.y"
    { (yyval.assignchaine) = new AssignChaine((yyvsp[(1) - (4)].variable),(yyvsp[(3) - (4)].chaine)) ; }
    break;

  case 46:
#line 301 "vrl.y"
    { (yyval.assignexpr) = new AssignExpr((yyvsp[(1) - (4)].variable),(yyvsp[(3) - (4)].calculableexpr)) ; }
    break;

  case 47:
#line 304 "vrl.y"
    { (yyval.assignvariable) = new AssignVariable((yyvsp[(1) - (4)].variable), (yyvsp[(3) - (4)].variable)); }
    break;

  case 48:
#line 307 "vrl.y"
    { (yyval.assignnouvelobjet) = new AssignNouvelObjet((yyvsp[(1) - (4)].variable),(yyvsp[(3) - (4)].nouvelobjet)) ; }
    break;

  case 49:
#line 310 "vrl.y"
    { (yyval.boucleif) = (yyvsp[(1) - (1)].boucleif) ; }
    break;

  case 50:
#line 312 "vrl.y"
    { (yyval.boucleif) = (yyvsp[(1) - (1)].boucleif) ; }
    break;

  case 51:
#line 315 "vrl.y"
    { (yyval.boucleif) = new BoucleIf((yyvsp[(2) - (5)].exprlogique),(yyvsp[(4) - (5)].instructionliste)) ; }
    break;

  case 52:
#line 318 "vrl.y"
    { (yyval.boucleif) = new BoucleIf((yyvsp[(2) - (9)].exprlogique),(yyvsp[(4) - (9)].instructionliste),(yyvsp[(8) - (9)].instructionliste)) ; }
    break;

  case 53:
#line 321 "vrl.y"
    { (yyval.bouclefor) = new BoucleFor((yyvsp[(3) - (10)].assignexpr), (yyvsp[(4) - (10)].exprlogique), (yyvsp[(6) - (10)].assignexpr), (yyvsp[(9) - (10)].instructionliste)) ; }
    break;

  case 54:
#line 324 "vrl.y"
    { (yyval.exprliste) = new ExprListe((yyvsp[(1) - (1)].expr)) ; }
    break;

  case 55:
#line 326 "vrl.y"
    { (yyvsp[(1) - (3)].exprliste)->addExpr((yyvsp[(3) - (3)].expr)) ; }
    break;

  case 56:
#line 329 "vrl.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].calculableexpr) ; }
    break;

  case 57:
#line 331 "vrl.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].variable) ; }
    break;

  case 58:
#line 334 "vrl.y"
    { (yyval.calculableexpr) = (yyvsp[(1) - (1)].nombre) ; }
    break;

  case 59:
#line 336 "vrl.y"
    { (yyval.calculableexpr) = new AppelDeFonction((yyvsp[(2) - (4)].chaine)) ; }
    break;

  case 60:
#line 338 "vrl.y"
    { (yyval.calculableexpr) = new AppelDeFonction((yyvsp[(2) - (5)].chaine), (yyvsp[(4) - (5)].exprliste)) ; }
    break;

  case 61:
#line 340 "vrl.y"
    { (yyval.calculableexpr) = (yyvsp[(1) - (1)].operationbinaire) ; }
    break;

  case 62:
#line 342 "vrl.y"
    { (yyval.calculableexpr) = (yyvsp[(1) - (1)].operationunaire) ; }
    break;

  case 63:
#line 344 "vrl.y"
    { (yyval.calculableexpr) = new ExprCalculable((yyvsp[(2) - (3)].expr)) ; }
    break;

  case 64:
#line 347 "vrl.y"
    { (yyval.exprlogique) = (yyvsp[(1) - (1)].comparaison) ; }
    break;

  case 65:
#line 349 "vrl.y"
    { (yyval.exprlogique) = (yyvsp[(1) - (1)].operationbinairelogique) ; }
    break;

  case 66:
#line 351 "vrl.y"
    { (yyval.exprlogique) = (yyvsp[(1) - (1)].operationunairelogique) ; }
    break;

  case 67:
#line 353 "vrl.y"
    { (yyval.exprlogique) = (yyvsp[(2) - (3)].exprlogique) ; }
    break;

  case 68:
#line 356 "vrl.y"
    {/* erreur ici */ (yyval.nouvelobjet) = new NouvelObjet((yyvsp[(2) - (4)].chaine)) ; }
    break;

  case 69:
#line 358 "vrl.y"
    {/* erreur ici */ (yyval.nouvelobjet) = new NouvelObjet((yyvsp[(2) - (5)].chaine), (yyvsp[(4) - (5)].exprliste)) ; }
    break;

  case 70:
#line 361 "vrl.y"
    { (yyval.variable) = (yyvsp[(1) - (1)].variablesimple) ; }
    break;

  case 71:
#line 363 "vrl.y"
    { (yyval.variable) = (yyvsp[(1) - (1)].variablecomplexe) ; }
    break;

  case 72:
#line 366 "vrl.y"
    { (yyval.variablesimple) = new Nom((yyvsp[(1) - (1)].chaine)) ; }
    break;

  case 73:
#line 368 "vrl.y"
    { (yyval.variablesimple) = (yyvsp[(1) - (1)].nomdetableau) ; }
    break;

  case 74:
#line 373 "vrl.y"
    { (yyval.nomdetableau) = new NomDeTableau((yyvsp[(1) - (4)].chaine), (yyvsp[(3) - (4)].expr)) ; }
    break;

  case 75:
#line 376 "vrl.y"
    { (yyval.variablecomplexe) = new VariableComplexe((yyvsp[(1) - (3)].variablesimple), (yyvsp[(3) - (3)].variablesimple)) ; }
    break;

  case 76:
#line 378 "vrl.y"
    { (yyvsp[(1) - (3)].variablecomplexe)->addNomComposant((yyvsp[(3) - (3)].variablesimple)) ; }
    break;

  case 77:
#line 381 "vrl.y"
    { (yyval.nomliste) = new NomListe((yyvsp[(1) - (1)].chaine)) ; }
    break;

  case 78:
#line 383 "vrl.y"
    { (yyvsp[(1) - (3)].nomliste)->addNom((yyvsp[(3) - (3)].chaine)) ; }
    break;

  case 79:
#line 386 "vrl.y"
    { (yyval.operationunaire) = new Negation((yyvsp[(3) - (4)].expr)) ; }
    break;

  case 80:
#line 388 "vrl.y"
    { (yyval.operationunaire) = new Absolu((yyvsp[(3) - (4)].expr)) ; }
    break;

  case 81:
#line 391 "vrl.y"
    { (yyval.operationbinaire) = new Addition((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)) ; }
    break;

  case 82:
#line 393 "vrl.y"
    { (yyval.operationbinaire) = new Soustraction((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)) ; }
    break;

  case 83:
#line 395 "vrl.y"
    { (yyval.operationbinaire) = new Multiplication((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)) ; }
    break;

  case 84:
#line 397 "vrl.y"
    { (yyval.operationbinaire) = new Division((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)) ; }
    break;

  case 85:
#line 400 "vrl.y"
    { (yyval.comparaison) = new Superieur((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)) ; }
    break;

  case 86:
#line 402 "vrl.y"
    { (yyval.comparaison) = new Inferieur((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)) ; }
    break;

  case 87:
#line 404 "vrl.y"
    { (yyval.comparaison) = new SuperieurOuEgal((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)) ; }
    break;

  case 88:
#line 406 "vrl.y"
    { (yyval.comparaison) = new InferieurOuEgal((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)) ; }
    break;

  case 89:
#line 408 "vrl.y"
    { (yyval.comparaison) = new EgalEgal((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)) ; }
    break;

  case 90:
#line 410 "vrl.y"
    { (yyval.comparaison) = new Different((yyvsp[(1) - (3)].expr), (yyvsp[(3) - (3)].expr)) ; }
    break;

  case 91:
#line 413 "vrl.y"
    { (yyval.operationunairelogique) = new NegationLogique((yyvsp[(2) - (2)].exprlogique)) ; }
    break;

  case 92:
#line 416 "vrl.y"
    { (yyval.operationbinairelogique) = new OuLogique((yyvsp[(1) - (3)].exprlogique), (yyvsp[(3) - (3)].exprlogique)) ; }
    break;

  case 93:
#line 418 "vrl.y"
    { (yyval.operationbinairelogique) = new EtLogique((yyvsp[(1) - (3)].exprlogique), (yyvsp[(3) - (3)].exprlogique)) ; }
    break;

  case 94:
#line 421 "vrl.y"
    { (yyval.nombre) = new Nombre((yyvsp[(1) - (1)].valeurint)) ; }
    break;

  case 95:
#line 423 "vrl.y"
    { (yyval.nombre) = new Nombre((yyvsp[(1) - (1)].valeurfloat)) ; }
    break;

  case 96:
#line 426 "vrl.y"
    {}
    break;


/* Line 1267 of yacc.c.  */
#line 2182 "y.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 428 "vrl.y"


void yyerror(char *s)
{
  fprintf(stderr, "### Error : %s\n", s) ;
  fprintf(stderr, "### Line=%d, Char=%dth : ''%s''\n", lineNumber, charPos, yytext) ;
  exit(-1) ;
}

