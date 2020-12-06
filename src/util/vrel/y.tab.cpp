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




/* Copy the first part of user declarations.  */
#line 1 "vrel.y"

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
/* Line 193 of yacc.c.  */
#line 285 "y.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 298 "y.tab.c"

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
#define YYLAST   249

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  47
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  22
/* YYNRULES -- Number of rules.  */
#define YYNRULES  68
/* YYNRULES -- Number of states.  */
#define YYNSTATES  155

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   301

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
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     9,    12,    15,    23,    27,    33,
      35,    38,    46,    54,    56,    59,    68,    70,    73,    75,
      78,    80,    86,    88,    95,   104,   112,   118,   122,   127,
     134,   136,   138,   140,   144,   146,   148,   150,   154,   158,
     162,   166,   170,   175,   178,   180,   184,   188,   192,   195,
     197,   199,   201,   205,   209,   213,   217,   221,   225,   227,
     232,   234,   236,   240,   242,   244,   248,   250,   252
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      48,     0,    -1,    68,    -1,    48,    12,     6,    -1,    48,
      49,    -1,    48,    51,    -1,    48,    10,    42,    50,    55,
      43,    11,    -1,     9,    64,    44,    -1,     9,    64,    25,
      60,    44,    -1,    68,    -1,    50,    49,    -1,    18,     5,
      19,     5,    42,    54,    43,    -1,    18,     5,    17,    42,
      50,    52,    43,    -1,    68,    -1,    52,    53,    -1,    20,
      38,    65,    39,    42,    50,    55,    43,    -1,    68,    -1,
      54,    59,    -1,    68,    -1,    55,    56,    -1,    59,    -1,
      15,    38,    62,    39,    58,    -1,    57,    -1,    16,     5,
      38,    66,    39,    44,    -1,    64,    25,    16,     5,    38,
      66,    39,    44,    -1,    13,    38,    62,    39,    58,    14,
      58,    -1,    13,    38,    62,    39,    58,    -1,    42,    55,
      43,    -1,    64,    25,    60,    44,    -1,     5,    46,     5,
      25,    60,    44,    -1,    64,    -1,    67,    -1,     6,    -1,
      38,    60,    39,    -1,    61,    -1,     7,    -1,     8,    -1,
      60,    26,    60,    -1,    60,    27,    60,    -1,    60,    28,
      60,    -1,    60,    29,    60,    -1,    60,    30,    60,    -1,
      31,    38,    60,    39,    -1,    27,    60,    -1,    63,    -1,
      38,    63,    39,    -1,    62,    23,    62,    -1,    62,    24,
      62,    -1,    22,    62,    -1,     8,    -1,     7,    -1,    64,
      -1,    60,    36,    60,    -1,    60,    32,    60,    -1,    60,
      33,    60,    -1,    60,    34,    60,    -1,    60,    35,    60,
      -1,    60,    37,    60,    -1,     5,    -1,     5,    40,    60,
      41,    -1,    68,    -1,     5,    -1,    65,    45,     5,    -1,
      68,    -1,    60,    -1,    66,    45,    60,    -1,     3,    -1,
       4,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   177,   177,   179,   181,   183,   185,   188,   190,   193,
     195,   198,   200,   203,   205,   208,   211,   213,   216,   218,
     221,   223,   225,   227,   229,   232,   234,   237,   240,   242,
     245,   247,   249,   251,   253,   255,   257,   260,   262,   264,
     266,   268,   270,   272,   275,   277,   279,   281,   283,   285,
     287,   289,   292,   294,   296,   298,   300,   302,   305,   307,
     310,   312,   314,   317,   319,   321,   324,   326,   329
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ENTIER", "REEL", "NOM", "CHAINE",
  "TRUE", "FALSE", "VAR", "BEGINKEY", "END", "IMPORT", "IF", "ELSE",
  "WHILE", "NEW", "COMPOUND", "CLASSE", "EXTENDS", "CONSTRUCTOR", "THIS",
  "NOT", "AND", "OR", "EGAL", "PLUS", "MOINS", "MULT", "DIV", "MOD", "ABS",
  "SUP", "INF", "SUPEGAL", "INFEGAL", "EGALEGAL", "DIFFERENT",
  "PARENTOUVR", "PARENTFERM", "CROCHOUVR", "CROCHFERM", "ACCOUVR",
  "ACCFERM", "PTVIRG", "VIRGULE", "POINT", "$accept", "PROGRAMME",
  "DECLARATION_VAR", "DECLARATION_VAR_LISTE", "DECLARATION_CLASSE",
  "DEFINITION_CLASSE", "CONSTRUCTEUR", "AFFECTATION_CLASSE_LISTE",
  "INSTRUCTION_LISTE", "INSTRUCTION", "INSTRUCTION_IF", "CONSEQUENCE",
  "AFFECTATION", "DATA", "OPERATION", "TEST", "COMPARAISON", "VARIABLE",
  "NOMS", "DATALISTE", "NOMBRE", "VIDE", 0
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
     295,   296,   297,   298,   299,   300,   301
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    47,    48,    48,    48,    48,    48,    49,    49,    50,
      50,    51,    51,    52,    52,    53,    54,    54,    55,    55,
      56,    56,    56,    56,    56,    57,    57,    58,    59,    59,
      60,    60,    60,    60,    60,    60,    60,    61,    61,    61,
      61,    61,    61,    61,    62,    62,    62,    62,    62,    62,
      62,    62,    63,    63,    63,    63,    63,    63,    64,    64,
      65,    65,    65,    66,    66,    66,    67,    67,    68
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     2,     2,     7,     3,     5,     1,
       2,     7,     7,     1,     2,     8,     1,     2,     1,     2,
       1,     5,     1,     6,     8,     7,     5,     3,     4,     6,
       1,     1,     1,     3,     1,     1,     1,     3,     3,     3,
       3,     3,     4,     2,     1,     3,     3,     3,     2,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     1,     4,
       1,     1,     3,     1,     1,     3,     1,     1,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
      68,     0,     2,     1,     0,     0,     0,     0,     4,     5,
      58,     0,    68,     3,     0,     0,     0,     7,    68,     9,
       0,     0,    66,    67,    32,    35,    36,     0,     0,     0,
       0,    34,    30,    31,     0,    10,     0,    18,    68,     0,
      43,     0,     0,     0,     0,     0,     0,     0,    59,     8,
      58,     0,     0,     0,     0,    19,    22,    20,     0,    68,
      68,     0,    33,    37,    38,    39,    40,    41,     0,     0,
       0,     0,     6,     0,     0,    13,     0,    16,    42,     0,
      35,    36,     0,     0,     0,     0,    44,    30,     0,    68,
       0,     0,     0,    12,    14,    11,    17,     0,     0,    48,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    64,     0,    63,     0,    28,    68,     0,     0,
      45,    53,    54,    55,    56,    52,    57,    46,    47,    68,
      26,    21,     0,     0,    68,    61,     0,    60,    29,     0,
       0,    23,    65,     0,     0,     0,    27,    25,     0,    68,
      62,    24,    68,     0,    15
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    35,    18,     9,    74,    94,    76,    36,    55,
      56,   130,    57,    84,    31,    85,    86,    32,   136,   113,
      33,    19
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -122
static const yytype_int16 yypact[] =
{
    -122,    75,  -122,  -122,     7,   -29,    16,    19,  -122,  -122,
      -5,   -21,  -122,  -122,    85,   151,   151,  -122,    31,  -122,
     -17,    53,  -122,  -122,  -122,  -122,  -122,   151,    25,   151,
     169,  -122,  -122,  -122,    42,  -122,     1,  -122,  -122,    32,
    -122,   151,   177,   151,   151,   151,   151,   151,  -122,  -122,
      36,    43,    57,   109,   105,  -122,  -122,  -122,   102,    31,
    -122,   199,  -122,    62,    62,  -122,  -122,  -122,   113,    93,
      93,    90,  -122,   137,   -15,  -122,     4,  -122,  -122,   108,
     -13,    15,    93,   151,   207,    38,  -122,    41,    98,   151,
     125,   143,    97,  -122,  -122,  -122,  -122,   121,   151,  -122,
     185,   110,   151,   151,   151,   151,   151,   151,    93,    93,
     106,   106,   219,    -2,  -122,   122,  -122,   142,   151,   164,
    -122,   219,   219,   219,   219,   219,   219,  -122,  -122,  -122,
     147,  -122,   107,   151,   151,  -122,    80,  -122,  -122,    14,
     106,  -122,   219,    87,   120,   158,  -122,  -122,   123,  -122,
    -122,  -122,    31,    40,  -122
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -122,  -122,   165,   -36,  -122,  -122,  -122,  -122,  -121,  -122,
    -122,  -104,    89,     5,  -122,   -67,    91,    -3,  -122,    45,
    -122,     0
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -52
static const yytype_int16 yytable[] =
{
       2,    11,    59,    88,    16,    92,    50,   131,   139,    50,
     -50,   -50,    10,    12,    51,    99,    52,    53,    37,    50,
      30,    34,    13,    17,    14,    38,   -50,    51,    93,    52,
      53,   153,    40,    58,    42,    15,   147,   132,   -49,   -49,
       4,   127,   128,   133,    54,    50,    61,    95,    63,    64,
      65,    66,    67,    51,   -49,    52,    53,   146,    39,    75,
      77,   108,   109,    41,   -51,   -51,    87,    87,    43,    44,
      45,    46,    47,    97,    60,     3,    15,   110,    91,    87,
     -51,    69,    68,   154,     4,     5,    49,     6,   100,   114,
      45,    46,    47,     7,   112,    70,    22,    23,    10,    24,
      80,    81,    20,   119,    21,    87,    87,   121,   122,   123,
     124,   125,   126,   152,    71,    82,    72,   137,    79,   144,
      27,   108,   109,    91,    28,   145,   148,    73,    89,    37,
     115,    83,   133,    98,   114,   117,    58,   111,   142,   112,
      22,    23,    10,    24,    25,    26,   118,   135,   129,   120,
      58,   141,    37,    90,    22,    23,    10,    24,    25,    26,
     134,   140,   149,   150,    27,    96,     8,   151,    28,    43,
      44,    45,    46,    47,   101,    29,     0,     0,    27,   143,
       0,     0,    28,     0,     0,     0,     0,   116,     0,    29,
      43,    44,    45,    46,    47,    43,    44,    45,    46,    47,
       0,     0,     0,    43,    44,    45,    46,    47,   138,     0,
      48,    43,    44,    45,    46,    47,    62,   102,   103,   104,
     105,   106,   107,     0,    62,    43,    44,    45,    46,    47,
       0,     0,     0,    43,    44,    45,    46,    47,    78,   102,
     103,   104,   105,   106,   107,    43,    44,    45,    46,    47
};

static const yytype_int16 yycheck[] =
{
       0,     4,    38,    70,    25,    20,     5,   111,   129,     5,
      23,    24,     5,    42,    13,    82,    15,    16,    18,     5,
      15,    16,     6,    44,     5,    42,    39,    13,    43,    15,
      16,   152,    27,    36,    29,    40,   140,    39,    23,    24,
       9,   108,   109,    45,    43,     5,    41,    43,    43,    44,
      45,    46,    47,    13,    39,    15,    16,    43,     5,    59,
      60,    23,    24,    38,    23,    24,    69,    70,    26,    27,
      28,    29,    30,    76,    42,     0,    40,    39,    73,    82,
      39,    38,    46,    43,     9,    10,    44,    12,    83,    89,
      28,    29,    30,    18,    89,    38,     3,     4,     5,     6,
       7,     8,    17,    98,    19,   108,   109,   102,   103,   104,
     105,   106,   107,   149,     5,    22,    11,   117,     5,    39,
      27,    23,    24,   118,    31,    45,    39,    25,    38,   129,
       5,    38,    45,    25,   134,    38,   139,    39,   133,   134,
       3,     4,     5,     6,     7,     8,    25,     5,    42,    39,
     153,    44,   152,    16,     3,     4,     5,     6,     7,     8,
      38,    14,    42,     5,    27,    76,     1,    44,    31,    26,
      27,    28,    29,    30,    83,    38,    -1,    -1,    27,   134,
      -1,    -1,    31,    -1,    -1,    -1,    -1,    44,    -1,    38,
      26,    27,    28,    29,    30,    26,    27,    28,    29,    30,
      -1,    -1,    -1,    26,    27,    28,    29,    30,    44,    -1,
      41,    26,    27,    28,    29,    30,    39,    32,    33,    34,
      35,    36,    37,    -1,    39,    26,    27,    28,    29,    30,
      -1,    -1,    -1,    26,    27,    28,    29,    30,    39,    32,
      33,    34,    35,    36,    37,    26,    27,    28,    29,    30
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    48,    68,     0,     9,    10,    12,    18,    49,    51,
       5,    64,    42,     6,     5,    40,    25,    44,    50,    68,
      17,    19,     3,     4,     6,     7,     8,    27,    31,    38,
      60,    61,    64,    67,    60,    49,    55,    68,    42,     5,
      60,    38,    60,    26,    27,    28,    29,    30,    41,    44,
       5,    13,    15,    16,    43,    56,    57,    59,    64,    50,
      42,    60,    39,    60,    60,    60,    60,    60,    46,    38,
      38,     5,    11,    25,    52,    68,    54,    68,    39,     5,
       7,     8,    22,    38,    60,    62,    63,    64,    62,    38,
      16,    60,    20,    43,    53,    43,    59,    64,    25,    62,
      60,    63,    32,    33,    34,    35,    36,    37,    23,    24,
      39,    39,    60,    66,    68,     5,    44,    38,    25,    60,
      39,    60,    60,    60,    60,    60,    60,    62,    62,    42,
      58,    58,    39,    45,    38,     5,    65,    68,    44,    55,
      14,    44,    60,    66,    39,    45,    43,    58,    39,    42,
       5,    44,    50,    55,    43
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
#line 178 "vrel.y"
    { (yyval.programme) = new Programme() ; prog = (yyval.programme) ; /* Nouveau programme */ }
    break;

  case 3:
#line 180 "vrel.y"
    { (yyvsp[(1) - (3)].programme)->import((yyvsp[(3) - (3)].chaine)) ; /* Importation d'un autre fichier */ }
    break;

  case 4:
#line 182 "vrel.y"
    { (yyvsp[(1) - (2)].programme)->adddeclarvar((yyvsp[(2) - (2)].declaration_var)) ;  /* Declaration de variable globale effectuee */ }
    break;

  case 5:
#line 184 "vrel.y"
    { /* Declaration de classe effectuee */ }
    break;

  case 6:
#line 186 "vrel.y"
    { (yyval.programme)->addmain((yyvsp[(4) - (7)].declaration_var_liste), (yyvsp[(5) - (7)].instruction_liste)) ; /* Fin du main */ }
    break;

  case 7:
#line 189 "vrel.y"
    { (yyval.declaration_var) = new Decl_var((yyvsp[(2) - (3)].variable)) ; /* Declaration simple */ }
    break;

  case 8:
#line 191 "vrel.y"
    { (yyval.declaration_var) = new Decl_var((yyvsp[(2) - (5)].variable), (yyvsp[(4) - (5)].data)) ; /* Declaration et affectation */ }
    break;

  case 9:
#line 194 "vrel.y"
    { (yyval.declaration_var_liste) = new Decl_var_liste() ; /* Debut des declarations de variables */ }
    break;

  case 10:
#line 196 "vrel.y"
    { (yyvsp[(1) - (2)].declaration_var_liste)->adddeclarvar((yyvsp[(2) - (2)].declaration_var)) ;  /* Declaration de variable effectuee */ }
    break;

  case 11:
#line 199 "vrel.y"
    { (yyval.declaration_classe) = new Class_extends((yyvsp[(2) - (7)].chaine), (yyvsp[(4) - (7)].chaine), (yyvsp[(6) - (7)].affectation_classe_liste)) ; /* Classe heritee */ }
    break;

  case 12:
#line 201 "vrel.y"
    { (yyval.declaration_classe) = new Compound((yyvsp[(2) - (7)].chaine), (yyvsp[(5) - (7)].declaration_var_liste), (yyvsp[(6) - (7)].definition_classe)) ; /* Classe construite */ }
    break;

  case 13:
#line 204 "vrel.y"
    { (yyval.definition_classe) = new Env_constr() ; /* Debut de la definition de la classe */ }
    break;

  case 14:
#line 206 "vrel.y"
    { (yyvsp[(1) - (2)].definition_classe)->addConstr((yyvsp[(2) - (2)].constructeur)) ; /* Nouveau constructeur rentre */ }
    break;

  case 15:
#line 209 "vrel.y"
    { (yyval.constructeur) = new Constructeur((yyvsp[(3) - (8)].noms), (yyvsp[(6) - (8)].declaration_var_liste), (yyvsp[(7) - (8)].instruction_liste)) ;  /* Fin du constructeur */ }
    break;

  case 16:
#line 212 "vrel.y"
    { (yyval.affectation_classe_liste) = new Instruction_liste() ; /* Debut de l'affectation de classe */ }
    break;

  case 17:
#line 214 "vrel.y"
    { (yyvsp[(1) - (2)].affectation_classe_liste)->addInstruction((yyvsp[(2) - (2)].affectation)) ; /* Affectation de classe finie */ }
    break;

  case 18:
#line 217 "vrel.y"
    { (yyval.instruction_liste) = new Instruction_liste() ; /* Debut des instructions */ }
    break;

  case 19:
#line 219 "vrel.y"
    { (yyval.instruction_liste)->addInstruction((yyvsp[(2) - (2)].instruction)) ;  /* Fin de l'instruction */ }
    break;

  case 20:
#line 222 "vrel.y"
    { (yyval.instruction) = (yyvsp[(1) - (1)].affectation) ; /* Fin de l'affectation */ }
    break;

  case 21:
#line 224 "vrel.y"
    { (yyval.instruction) = new Boucle_while((yyvsp[(3) - (5)].test), (yyvsp[(5) - (5)].consequence)) ; /* Fin de la boucle_while */ }
    break;

  case 22:
#line 226 "vrel.y"
    { (yyval.instruction) = (yyvsp[(1) - (1)].instructionif) ;  /* Fin du test_if */ }
    break;

  case 23:
#line 228 "vrel.y"
    { (yyval.instruction) = new Creation((yyvsp[(2) - (6)].chaine), (yyvsp[(4) - (6)].dataliste)) ;  /* Fin du nouvel objet */ }
    break;

  case 24:
#line 230 "vrel.y"
    { (yyval.instruction) = new Creation((yyvsp[(1) - (8)].variable), (yyvsp[(4) - (8)].chaine), (yyvsp[(6) - (8)].dataliste)) ; /* Nouvel objet cree */ }
    break;

  case 25:
#line 233 "vrel.y"
    { (yyval.instructionif) = new Alternative((yyvsp[(3) - (7)].test), (yyvsp[(5) - (7)].consequence), (yyvsp[(7) - (7)].consequence)) ; /* If then else */ }
    break;

  case 26:
#line 235 "vrel.y"
    { (yyval.instructionif) = new Alternative((yyvsp[(3) - (5)].test), (yyvsp[(5) - (5)].consequence)) ; /* If then */ }
    break;

  case 27:
#line 238 "vrel.y"
    { (yyval.consequence) = (yyvsp[(2) - (3)].instruction_liste) ; /* Fin des consequences */ }
    break;

  case 28:
#line 241 "vrel.y"
    { (yyval.affectation) = new Affectation((yyvsp[(1) - (4)].variable), (yyvsp[(3) - (4)].data)) ; /* Affectation effectuee */ }
    break;

  case 29:
#line 243 "vrel.y"
    { (yyval.affectation) = new Affectation((yyvsp[(1) - (6)].chaine), (yyvsp[(3) - (6)].chaine), (yyvsp[(5) - (6)].data)) ; /* Element d'un objet */ }
    break;

  case 30:
#line 246 "vrel.y"
    { (yyval.data) = (yyvsp[(1) - (1)].variable) ; /* Data=variable */ }
    break;

  case 31:
#line 248 "vrel.y"
    { (yyval.data) = (yyvsp[(1) - (1)].nombre) ; /* Data=nombre */ }
    break;

  case 32:
#line 250 "vrel.y"
    { (yyval.data) = new Chaine((yyvsp[(1) - (1)].chaine)) ;  /* Data=chaine de caractères */ }
    break;

  case 33:
#line 252 "vrel.y"
    { (yyval.data) = (yyvsp[(2) - (3)].data) ; /* (data) */ }
    break;

  case 34:
#line 254 "vrel.y"
    { (yyval.data) = (yyvsp[(1) - (1)].operation) ; /* Data=operation */ }
    break;

  case 35:
#line 256 "vrel.y"
    { (yyval.data) = new Vrai() ; /* Data=vrai */ }
    break;

  case 36:
#line 258 "vrel.y"
    { (yyval.data) = new Faux() ; /* Data=faux */ }
    break;

  case 37:
#line 261 "vrel.y"
    { (yyval.operation) = new Addition      ((yyvsp[(1) - (3)].data), (yyvsp[(3) - (3)].data)) ; /* Addition */ }
    break;

  case 38:
#line 263 "vrel.y"
    { (yyval.operation) = new Soustraction  ((yyvsp[(1) - (3)].data), (yyvsp[(3) - (3)].data)) ; /* Soustraction */ }
    break;

  case 39:
#line 265 "vrel.y"
    { (yyval.operation) = new Multiplication((yyvsp[(1) - (3)].data), (yyvsp[(3) - (3)].data)) ; /* Multiplication */ }
    break;

  case 40:
#line 267 "vrel.y"
    { (yyval.operation) = new Division      ((yyvsp[(1) - (3)].data), (yyvsp[(3) - (3)].data)) ; /* Division */ }
    break;

  case 41:
#line 269 "vrel.y"
    { (yyval.operation) = new Modulo        ((yyvsp[(1) - (3)].data), (yyvsp[(3) - (3)].data)) ; /* Modulo */ }
    break;

  case 42:
#line 271 "vrel.y"
    { (yyval.operation) = new Valabsolue((yyvsp[(3) - (4)].data)) ; /* Valeur absolue */ }
    break;

  case 43:
#line 273 "vrel.y"
    { (yyval.operation) = new Opposition((yyvsp[(2) - (2)].data)) ; /* Opposee */ }
    break;

  case 44:
#line 276 "vrel.y"
    { (yyval.test) = (yyvsp[(1) - (1)].comparaison) ; /* Comparaison */ }
    break;

  case 45:
#line 278 "vrel.y"
    { (yyval.test) = (yyvsp[(2) - (3)].comparaison) ; /* Comparaison */ }
    break;

  case 46:
#line 280 "vrel.y"
    { (yyval.test) = new And((yyvsp[(1) - (3)].test), (yyvsp[(3) - (3)].test)) ; /* Test AND test */ }
    break;

  case 47:
#line 282 "vrel.y"
    { (yyval.test) = new Or ((yyvsp[(1) - (3)].test), (yyvsp[(3) - (3)].test)) ; /* Test OR test */ }
    break;

  case 48:
#line 284 "vrel.y"
    { (yyval.test) = new Not((yyvsp[(2) - (2)].test)) ;     /* NOT test */ }
    break;

  case 49:
#line 286 "vrel.y"
    { (yyval.test) = new Faux() ; /* FALSE */ }
    break;

  case 50:
#line 288 "vrel.y"
    { (yyval.test) = new Vrai() ; /* TRUE */ }
    break;

  case 51:
#line 290 "vrel.y"
    { (yyval.test) = (yyvsp[(1) - (1)].variable) ; }
    break;

  case 52:
#line 293 "vrel.y"
    { (yyval.comparaison) = new Egalite  ((yyvsp[(1) - (3)].data), (yyvsp[(3) - (3)].data)) ; /* Egal-egal */ }
    break;

  case 53:
#line 295 "vrel.y"
    { (yyval.comparaison) = new Superieur((yyvsp[(1) - (3)].data), (yyvsp[(3) - (3)].data)) ; /* Superieur */ }
    break;

  case 54:
#line 297 "vrel.y"
    { (yyval.comparaison) = new Inferieur((yyvsp[(1) - (3)].data), (yyvsp[(3) - (3)].data)) ; /* Inferieur */ }
    break;

  case 55:
#line 299 "vrel.y"
    { (yyval.comparaison) = new Supegal  ((yyvsp[(1) - (3)].data), (yyvsp[(3) - (3)].data)) ; /* Superieur ou Egal */ }
    break;

  case 56:
#line 301 "vrel.y"
    { (yyval.comparaison) = new Infegal  ((yyvsp[(1) - (3)].data), (yyvsp[(3) - (3)].data)) ; /* Inferieur ou Egal */ }
    break;

  case 57:
#line 303 "vrel.y"
    { (yyval.comparaison) = new Different((yyvsp[(1) - (3)].data), (yyvsp[(3) - (3)].data)) ; /* Different */ }
    break;

  case 58:
#line 306 "vrel.y"
    { (yyval.variable) = new Variable((yyvsp[(1) - (1)].chaine)) ; /* Variable simple */ }
    break;

  case 59:
#line 308 "vrel.y"
    {/* $$ = new Variable($1, $3) ; */ /* Tableau : non implemente */ }
    break;

  case 60:
#line 311 "vrel.y"
    { (yyval.noms) = new Nom_liste() ; /* Liste vide */ }
    break;

  case 61:
#line 313 "vrel.y"
    { (yyval.noms) = new Nom_liste() ; (yyval.noms)->ajouter((yyvsp[(1) - (1)].chaine)) ; /* Liste simple */ }
    break;

  case 62:
#line 315 "vrel.y"
    { (yyvsp[(1) - (3)].noms)->ajouter((yyvsp[(3) - (3)].chaine)) ; /* Variable simple */ }
    break;

  case 63:
#line 318 "vrel.y"
    { (yyval.dataliste) = new Data_liste() ; /* Liste2 vide */ }
    break;

  case 64:
#line 320 "vrel.y"
    { (yyval.dataliste) = new Data_liste() ; (yyval.dataliste)->ajouter((yyvsp[(1) - (1)].data)) ; /* Liste simple */ }
    break;

  case 65:
#line 322 "vrel.y"
    { (yyvsp[(1) - (3)].dataliste)->ajouter((yyvsp[(3) - (3)].data)) ; /* Variable simple */ }
    break;

  case 66:
#line 325 "vrel.y"
    { (yyval.nombre) = new Nombre((yyvsp[(1) - (1)].valeurint)) ; /* Entier */ }
    break;

  case 67:
#line 327 "vrel.y"
    { (yyval.nombre) = new Nombre((yyvsp[(1) - (1)].valeurfloat)) ; /* Reel */ }
    break;

  case 68:
#line 329 "vrel.y"
    {}
    break;


/* Line 1267 of yacc.c.  */
#line 1977 "y.tab.c"
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


#line 331 "vrel.y"


void yyerror(char *s)
{
  printf("syntax error: %s\n", s);
  printf("File: %s, line %d: \"%s\"\n", vrelfile, yylineno, yytext);
  exit(0);
}

