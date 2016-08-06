/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "vrel.y" /* yacc.c:339  */

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


#line 135 "y.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
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

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 71 "vrel.y" /* yacc.c:355  */

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

#line 295 "y.tab.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 312 "y.tab.c" /* yacc.c:358  */

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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
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
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

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
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  155

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   301

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
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
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
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

#if YYDEBUG || YYERROR_VERBOSE || 0
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
  "NOMS", "DATALISTE", "NOMBRE", "VIDE", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301
};
# endif

#define YYPACT_NINF -122

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-122)))

#define YYTABLE_NINF -52

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
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

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
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

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -122,  -122,   165,   -36,  -122,  -122,  -122,  -122,  -121,  -122,
    -122,  -104,    89,     5,  -122,   -67,    91,    -3,  -122,    45,
    -122,     0
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    35,    18,     9,    74,    94,    76,    36,    55,
      56,   130,    57,    84,    31,    85,    86,    32,   136,   113,
      33,    19
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
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

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
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


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
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
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
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
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
     '$$ = $1'.

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
#line 178 "vrel.y" /* yacc.c:1646  */
    { (yyval.programme) = new Programme() ; prog = (yyval.programme) ; /* Nouveau programme */ }
#line 1511 "y.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 180 "vrel.y" /* yacc.c:1646  */
    { (yyvsp[-2].programme)->import((yyvsp[0].chaine)) ; /* Importation d'un autre fichier */ }
#line 1517 "y.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 182 "vrel.y" /* yacc.c:1646  */
    { (yyvsp[-1].programme)->adddeclarvar((yyvsp[0].declaration_var)) ;  /* Déclaration de variable globale effectuée */ }
#line 1523 "y.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 184 "vrel.y" /* yacc.c:1646  */
    { /* Déclaration de classe effectuée */ }
#line 1529 "y.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 186 "vrel.y" /* yacc.c:1646  */
    { (yyval.programme)->addmain((yyvsp[-3].declaration_var_liste), (yyvsp[-2].instruction_liste)) ; /* Fin du main */ }
#line 1535 "y.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 189 "vrel.y" /* yacc.c:1646  */
    { (yyval.declaration_var) = new Decl_var((yyvsp[-1].variable)) ; /* Déclaration simple */ }
#line 1541 "y.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 191 "vrel.y" /* yacc.c:1646  */
    { (yyval.declaration_var) = new Decl_var((yyvsp[-3].variable), (yyvsp[-1].data)) ; /* Déclaration et affectation */ }
#line 1547 "y.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 194 "vrel.y" /* yacc.c:1646  */
    { (yyval.declaration_var_liste) = new Decl_var_liste() ; /* Début des déclarations de variables */ }
#line 1553 "y.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 196 "vrel.y" /* yacc.c:1646  */
    { (yyvsp[-1].declaration_var_liste)->adddeclarvar((yyvsp[0].declaration_var)) ;  /* Déclaration de variable effectuée */ }
#line 1559 "y.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 199 "vrel.y" /* yacc.c:1646  */
    { (yyval.declaration_classe) = new Class_extends((yyvsp[-5].chaine), (yyvsp[-3].chaine), (yyvsp[-1].affectation_classe_liste)) ; /* Classe héritée */ }
#line 1565 "y.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 201 "vrel.y" /* yacc.c:1646  */
    { (yyval.declaration_classe) = new Compound((yyvsp[-5].chaine), (yyvsp[-2].declaration_var_liste), (yyvsp[-1].definition_classe)) ; /* Classe construite */ }
#line 1571 "y.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 204 "vrel.y" /* yacc.c:1646  */
    { (yyval.definition_classe) = new Env_constr() ; /* Début de la définition de la classe */ }
#line 1577 "y.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 206 "vrel.y" /* yacc.c:1646  */
    { (yyvsp[-1].definition_classe)->addConstr((yyvsp[0].constructeur)) ; /* Nouveau constructeur rentré */ }
#line 1583 "y.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 209 "vrel.y" /* yacc.c:1646  */
    { (yyval.constructeur) = new Constructeur((yyvsp[-5].noms), (yyvsp[-2].declaration_var_liste), (yyvsp[-1].instruction_liste)) ;  /* Fin du constructeur */ }
#line 1589 "y.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 212 "vrel.y" /* yacc.c:1646  */
    { (yyval.affectation_classe_liste) = new Instruction_liste() ; /* Debut de l'affectation de classe */ }
#line 1595 "y.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 214 "vrel.y" /* yacc.c:1646  */
    { (yyvsp[-1].affectation_classe_liste)->addInstruction((yyvsp[0].affectation)) ; /* Affectation de classe finie */ }
#line 1601 "y.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 217 "vrel.y" /* yacc.c:1646  */
    { (yyval.instruction_liste) = new Instruction_liste() ; /* Début des instructions */ }
#line 1607 "y.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 219 "vrel.y" /* yacc.c:1646  */
    { (yyval.instruction_liste)->addInstruction((yyvsp[0].instruction)) ;  /* Fin de l'instruction */ }
#line 1613 "y.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 222 "vrel.y" /* yacc.c:1646  */
    { (yyval.instruction) = (yyvsp[0].affectation) ; /* Fin de l'affectation */ }
#line 1619 "y.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 224 "vrel.y" /* yacc.c:1646  */
    { (yyval.instruction) = new Boucle_while((yyvsp[-2].test), (yyvsp[0].consequence)) ; /* Fin de la boucle_while */ }
#line 1625 "y.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 226 "vrel.y" /* yacc.c:1646  */
    { (yyval.instruction) = (yyvsp[0].instructionif) ;  /* Fin du test_if */ }
#line 1631 "y.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 228 "vrel.y" /* yacc.c:1646  */
    { (yyval.instruction) = new Creation((yyvsp[-4].chaine), (yyvsp[-2].dataliste)) ;  /* Fin du nouvel objet */ }
#line 1637 "y.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 230 "vrel.y" /* yacc.c:1646  */
    { (yyval.instruction) = new Creation((yyvsp[-7].variable), (yyvsp[-4].chaine), (yyvsp[-2].dataliste)) ; /* Nouvel objet créé */ }
#line 1643 "y.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 233 "vrel.y" /* yacc.c:1646  */
    { (yyval.instructionif) = new Alternative((yyvsp[-4].test), (yyvsp[-2].consequence), (yyvsp[0].consequence)) ; /* If then else */ }
#line 1649 "y.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 235 "vrel.y" /* yacc.c:1646  */
    { (yyval.instructionif) = new Alternative((yyvsp[-2].test), (yyvsp[0].consequence)) ; /* If then */ }
#line 1655 "y.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 238 "vrel.y" /* yacc.c:1646  */
    { (yyval.consequence) = (yyvsp[-1].instruction_liste) ; /* Fin des conséquences */ }
#line 1661 "y.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 241 "vrel.y" /* yacc.c:1646  */
    { (yyval.affectation) = new Affectation((yyvsp[-3].variable), (yyvsp[-1].data)) ; /* Affectation effectuée */ }
#line 1667 "y.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 243 "vrel.y" /* yacc.c:1646  */
    { (yyval.affectation) = new Affectation((yyvsp[-5].chaine), (yyvsp[-3].chaine), (yyvsp[-1].data)) ; /* Elément d'un objet */ }
#line 1673 "y.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 246 "vrel.y" /* yacc.c:1646  */
    { (yyval.data) = (yyvsp[0].variable) ; /* Data=variable */ }
#line 1679 "y.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 248 "vrel.y" /* yacc.c:1646  */
    { (yyval.data) = (yyvsp[0].nombre) ; /* Data=nombre */ }
#line 1685 "y.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 250 "vrel.y" /* yacc.c:1646  */
    { (yyval.data) = new Chaine((yyvsp[0].chaine)) ;  /* Data=chaine de caractères */ }
#line 1691 "y.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 252 "vrel.y" /* yacc.c:1646  */
    { (yyval.data) = (yyvsp[-1].data) ; /* (data) */ }
#line 1697 "y.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 254 "vrel.y" /* yacc.c:1646  */
    { (yyval.data) = (yyvsp[0].operation) ; /* Data=opération */ }
#line 1703 "y.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 256 "vrel.y" /* yacc.c:1646  */
    { (yyval.data) = new Vrai() ; /* Data=vrai */ }
#line 1709 "y.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 258 "vrel.y" /* yacc.c:1646  */
    { (yyval.data) = new Faux() ; /* Data=faux */ }
#line 1715 "y.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 261 "vrel.y" /* yacc.c:1646  */
    { (yyval.operation) = new Addition      ((yyvsp[-2].data), (yyvsp[0].data)) ; /* Addition */ }
#line 1721 "y.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 263 "vrel.y" /* yacc.c:1646  */
    { (yyval.operation) = new Soustraction  ((yyvsp[-2].data), (yyvsp[0].data)) ; /* Soustraction */ }
#line 1727 "y.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 265 "vrel.y" /* yacc.c:1646  */
    { (yyval.operation) = new Multiplication((yyvsp[-2].data), (yyvsp[0].data)) ; /* Multiplication */ }
#line 1733 "y.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 267 "vrel.y" /* yacc.c:1646  */
    { (yyval.operation) = new Division      ((yyvsp[-2].data), (yyvsp[0].data)) ; /* Division */ }
#line 1739 "y.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 269 "vrel.y" /* yacc.c:1646  */
    { (yyval.operation) = new Modulo        ((yyvsp[-2].data), (yyvsp[0].data)) ; /* Modulo */ }
#line 1745 "y.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 271 "vrel.y" /* yacc.c:1646  */
    { (yyval.operation) = new Valabsolue((yyvsp[-1].data)) ; /* Valeur absolue */ }
#line 1751 "y.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 273 "vrel.y" /* yacc.c:1646  */
    { (yyval.operation) = new Opposition((yyvsp[0].data)) ; /* Opposee */ }
#line 1757 "y.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 276 "vrel.y" /* yacc.c:1646  */
    { (yyval.test) = (yyvsp[0].comparaison) ; /* Comparaison */ }
#line 1763 "y.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 278 "vrel.y" /* yacc.c:1646  */
    { (yyval.test) = (yyvsp[-1].comparaison) ; /* Comparaison */ }
#line 1769 "y.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 280 "vrel.y" /* yacc.c:1646  */
    { (yyval.test) = new And((yyvsp[-2].test), (yyvsp[0].test)) ; /* Test AND test */ }
#line 1775 "y.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 282 "vrel.y" /* yacc.c:1646  */
    { (yyval.test) = new Or ((yyvsp[-2].test), (yyvsp[0].test)) ; /* Test OR test */ }
#line 1781 "y.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 284 "vrel.y" /* yacc.c:1646  */
    { (yyval.test) = new Not((yyvsp[0].test)) ;     /* NOT test */ }
#line 1787 "y.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 286 "vrel.y" /* yacc.c:1646  */
    { (yyval.test) = new Faux() ; /* FALSE */ }
#line 1793 "y.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 288 "vrel.y" /* yacc.c:1646  */
    { (yyval.test) = new Vrai() ; /* TRUE */ }
#line 1799 "y.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 290 "vrel.y" /* yacc.c:1646  */
    { (yyval.test) = (yyvsp[0].variable) ; }
#line 1805 "y.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 293 "vrel.y" /* yacc.c:1646  */
    { (yyval.comparaison) = new Egalite  ((yyvsp[-2].data), (yyvsp[0].data)) ; /* Egal-egal */ }
#line 1811 "y.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 295 "vrel.y" /* yacc.c:1646  */
    { (yyval.comparaison) = new Superieur((yyvsp[-2].data), (yyvsp[0].data)) ; /* Supérieur */ }
#line 1817 "y.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 297 "vrel.y" /* yacc.c:1646  */
    { (yyval.comparaison) = new Inferieur((yyvsp[-2].data), (yyvsp[0].data)) ; /* Inférieur */ }
#line 1823 "y.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 299 "vrel.y" /* yacc.c:1646  */
    { (yyval.comparaison) = new Supegal  ((yyvsp[-2].data), (yyvsp[0].data)) ; /* Supérieur ou Egal */ }
#line 1829 "y.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 301 "vrel.y" /* yacc.c:1646  */
    { (yyval.comparaison) = new Infegal  ((yyvsp[-2].data), (yyvsp[0].data)) ; /* Inférieur ou Egal */ }
#line 1835 "y.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 303 "vrel.y" /* yacc.c:1646  */
    { (yyval.comparaison) = new Different((yyvsp[-2].data), (yyvsp[0].data)) ; /* Différent */ }
#line 1841 "y.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 306 "vrel.y" /* yacc.c:1646  */
    { (yyval.variable) = new Variable((yyvsp[0].chaine)) ; /* Variable simple */ }
#line 1847 "y.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 308 "vrel.y" /* yacc.c:1646  */
    {/* $$ = new Variable($1, $3) ; */ /* Tableau : non implemente */ }
#line 1853 "y.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 311 "vrel.y" /* yacc.c:1646  */
    { (yyval.noms) = new Nom_liste() ; /* Liste vide */ }
#line 1859 "y.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 313 "vrel.y" /* yacc.c:1646  */
    { (yyval.noms) = new Nom_liste() ; (yyval.noms)->ajouter((yyvsp[0].chaine)) ; /* Liste simple */ }
#line 1865 "y.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 315 "vrel.y" /* yacc.c:1646  */
    { (yyvsp[-2].noms)->ajouter((yyvsp[0].chaine)) ; /* Variable simple */ }
#line 1871 "y.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 318 "vrel.y" /* yacc.c:1646  */
    { (yyval.dataliste) = new Data_liste() ; /* Liste2 vide */ }
#line 1877 "y.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 320 "vrel.y" /* yacc.c:1646  */
    { (yyval.dataliste) = new Data_liste() ; (yyval.dataliste)->ajouter((yyvsp[0].data)) ; /* Liste simple */ }
#line 1883 "y.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 322 "vrel.y" /* yacc.c:1646  */
    { (yyvsp[-2].dataliste)->ajouter((yyvsp[0].data)) ; /* Variable simple */ }
#line 1889 "y.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 325 "vrel.y" /* yacc.c:1646  */
    { (yyval.nombre) = new Nombre((yyvsp[0].valeurint)) ; /* Entier */ }
#line 1895 "y.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 327 "vrel.y" /* yacc.c:1646  */
    { (yyval.nombre) = new Nombre((yyvsp[0].valeurfloat)) ; /* Réel */ }
#line 1901 "y.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 329 "vrel.y" /* yacc.c:1646  */
    {}
#line 1907 "y.tab.c" /* yacc.c:1646  */
    break;


#line 1911 "y.tab.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
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

  /* Else will try to reuse lookahead token after shifting the error
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

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return yyresult;
}
#line 331 "vrel.y" /* yacc.c:1906  */


void yyerror(char *s)
{
  printf("syntax error: %s\n", s);
  printf("File: %s, line %d: \"%s\"\n", vrelfile, yylineno, yytext);
  exit(0);
}
