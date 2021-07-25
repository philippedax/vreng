%{
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "input.hpp"

#define YYDEBUG 1
//extern int yy_flex_debug;  // pour le debogage

extern FILE* yyin;

int yylineno;
extern int yyerror(char*);
extern int yylex();

void mix_texture(Tex* t1, Tex* t2);
void mix_app(App* a1, App* a2);
void moveCenter(Vect& center, Vect& size);
float deg(const float);

int curtype;

/* le groupe racine cree */ 
Group *gr;
/* pour obtenir les valeurs par defaut */
App *a;

struct structBoxProps {
  Vect* bp_box_size;
  Tex* bp_box_tex;
  App* bp_app;
};

struct structSpherProps {
  float sp_spher_size;
  char* sp_spher_tex;
  App* sp_app;
};

void free_box_props (struct structBoxProps *bp)
{
  if (bp->bp_box_size != NULL) delete(bp->bp_box_size);
  if (bp->bp_box_tex != NULL) delete(bp->bp_box_tex);
  if (bp->bp_app != NULL) delete(bp->bp_app);
  free(bp);
}

void free_spher_props (struct structSpherProps *sp)
{
  if (sp->sp_spher_tex != NULL) free(sp->sp_spher_tex);
  if (sp->sp_app != NULL) delete(sp->sp_app);
  free(sp);
}

%}

%union {
  float dval;
  char*  sval;
  float* dpval;
  Vect* vectval;
  Tex*  texval;
  struct structBoxProps* bpval;
  struct structSpherProps* spval;
  App* appval;
}

%token COMMA
%token <dval> NUMBER
%token <sval> STRING
/* type d'objets */
%token TK_WALL TK_GATE TK_EARTH TK_WEB TK_BOARD TK_STEP TK_HOST TK_DOC TK_MIRAGE TK_THING
%token TK_END

%token TK_BOX_SIZE TK_SPHER_SIZE TK_TOR_SIZE TK_TOR_SIZE2
%token <dval> TK_DIFFUSE TK_AMBIENT TK_SPECULAR TK_EMISSION TK_SHININESS
%token <sval> TEX_XP TEX_YP TEX_ZP TEX_XN TEX_YN TEX_ZN SPHER_TEX
%token <sval> TLNT

%type <dpval> pos_ang 
%type <bpval> box_props
%type <spval> spher_props
%type <vectval> box_size 
%type <dval> spher_size 
%type <texval>  box_tex box_tex_single
%type <appval>  app app_single

%%

start: solid TK_END

solid: solid_single | solid solid_single

solid_single:
  wall_section |
  gate_section |
  earth_section |
  web_section |
  board_section |
  step_section |
  host_section |
  doc_section
/** |
  mirage_section |
  thing_section
**/

/*************** Niveau 1 : Types *****************/

wall_section: TK_WALL wall
wall: wall_single | wall wall_single
wall_single: pos_ang box_props {
  Vect center($1[0], $1[1], $1[2]);
  Vect size(*($2->bp_box_size));
  size *= 2;
  Vect o(0,0,deg($1[3]));
  Tex* t = $2->bp_box_tex;
  App* a = $2->bp_app;
  Wall *w = new Wall("myWall", center, o, size, TEXTURED, Color::white, *t, *a);
  gr->add(w);
  free_box_props($2); // libere aussi les objets pointes
  delete[]($1);
}

gate_section: TK_GATE gate
gate: gate_single | gate gate_single
gate_single: pos_ang STRING STRING box_props {
  Vect center($1[0], $1[1], $1[2]);
  Vect size(*($4->bp_box_size));
  size *= 2;
  Vect o(0,0,deg($1[3]));
  Tex* t = $4->bp_box_tex;
  App* a = $4->bp_app;
  Gate *g = new Gate("myWall", center, o, size, TEXTURED, Color::white, *t, *a,
		     $2, $3);
  gr->add(g);
  cout << *g;
  free_box_props($4);
  delete[]($1);
}

earth_section: TK_EARTH earth
earth: earth_single | earth earth_single
earth_single: pos_ang spher_props {
  Vect center($1[0], $1[1], $1[2]);
  Vect size($2->sp_spher_size,$2->sp_spher_size,$2->sp_spher_size);
  App* a = $2->sp_app;
  Earth *s = new Earth("mySphere", center, Vect::null, size, TEXTURED,
		       Color::white, Tex($2->sp_spher_tex), *a);
  gr->add(s);
  cout << *s;
  free_spher_props($2);
  delete[]($1);
}

web_section: TK_WEB web
web: web_single | web web_single
web_single: pos_ang STRING box_props {
  Vect center($1[0], $1[1], $1[2]);
  Vect size(*($3->bp_box_size));
  size *= 2;
  Vect o(0,0,deg($1[3]));
  Tex* t = $3->bp_box_tex;
  App* a = $3->bp_app;
  Web *w = new Web("myWall", center, o, size, TEXTURED, Color::white, *t, *a,
                   $2);
  gr->add(w);
  free_box_props($3);
  delete[]($1);
}

board_section: TK_BOARD board
board: board_single | board board_single
board_single: pos_ang box_props {
  Vect center($1[0], $1[1], $1[2]);
  Vect size(*($2->bp_box_size));
  size *= 2;
  Vect o(0,0,deg($1[3]));
  Tex* t = $2->bp_box_tex;
  App* a = $2->bp_app;
  Board *b = new Board("myWall", center, o, size, TEXTURED, Color::white, *t, *a);
  gr->add(b);
  free_box_props($2);
  delete[]($1);
}

step_section: TK_STEP step
step: step_single | step step_single
step_single: pos_ang box_props {
  Vect center($1[0], $1[1], $1[2]);
  Vect size(*($2->bp_box_size));
  size *= 2;
  Vect o(0,0,deg($1[3]));
  Tex* t = $2->bp_box_tex;
  App* a = $2->bp_app;
  Step *s = new Step("myWall", center, o, size, TEXTURED, Color::white, *t, *a);
  gr->add(s);
  free_box_props($2);
  delete[]($1);
}

host_section: TK_HOST host
host: host_single | host host_single
host_single: pos_ang TLNT box_props {
  Vect center($1[0], $1[1], $1[2]);
  Vect size(*($3->bp_box_size));
  size *= 2;
  Vect o(0,0,deg($1[3]));
  Tex* t = $3->bp_box_tex == NULL ? new Tex() : new Tex(*($3->bp_box_tex));
  App* a = $3->bp_app == NULL ? new App() : new App(*($3->bp_app));
  Host *h = new Host("myWall", center, o, size, TEXTURED, Color::white, *t, *a,
                     $2);
  gr->add(h);
  free_box_props($3);
  delete[]($1);
}

doc_section: TK_DOC doc
doc: doc_single | doc doc_single
doc_single: pos_ang STRING box_props {
  Vect center($1[0], $1[1], $1[2]);
  Vect size(*($3->bp_box_size));
  size *= 2;
  Vect o(0,0,deg($1[3]));
  Tex* t = $3->bp_box_tex;
  App* a = $3->bp_app;
  Doc *d = new Doc("myWall", center, o, size, TEXTURED, Color::white, *t, *a,
                   $2);
  gr->add(d);
  free_box_props($3);
  delete[]($1);
}

mirage_section: TK_MIRAGE mirage
mirage: mirage_single | mirage mirage_single
mirage_single: pos_ang box_props {
  Vect center($1[0], $1[1], $1[2]);
  Vect size(*($2->bp_box_size));
  size *= 2;
  Vect o(0,0,deg($1[3]));
  Tex* t = $2->bp_box_tex;
  App* a = $2->bp_app;
  Mirage *m = new Mirage("myWall", center, o, size, TEXTURED, Color::white, *t, *a);
  gr->add(m);
  free_box_props($2);
  delete[]($1);
}

thing_section: TK_THING thing
thing: thing_single | thing thing_single
thing_single: pos_ang box_props {
  Vect center($1[0], $1[1], $1[2]);
  Vect size(*($2->bp_box_size));
  size *= 2;
  Vect o(0,0,deg($1[3]));
  Tex* t = $2->bp_box_tex;
  App* a = $2->bp_app;
  Thing *th = new Thing("myWall", center, o, size, TEXTURED, Color::white, *t, *a);
  gr->add(th);
  free_box_props($2);
  delete[]($1);
}

/******************** Niveau 2 : Composees **********************/

box_props:
   box_size COMMA box_tex COMMA app {
      struct structBoxProps *res;
      res = (struct structBoxProps *)malloc(sizeof(struct structBoxProps));
      res->bp_box_size = $1 ;
      res->bp_box_tex = $3;
      res->bp_app = $5;
      
      $$ = res;
   }
   | box_size COMMA box_tex {
      struct structBoxProps *res;
      res = (struct structBoxProps *)malloc(sizeof(struct structBoxProps));
      res->bp_box_size = $1 ;
      res->bp_box_tex = $3;
      res->bp_app = new App();
      
      $$ = res;
   }
   | box_size COMMA app {
      struct structBoxProps *res;
      res = (struct structBoxProps *)malloc(sizeof(struct structBoxProps));
      res->bp_box_size = $1 ;
      res->bp_box_tex = new Tex();
      res->bp_app = $3;
      
      $$ = res;
   }
   | box_size { 
      struct structBoxProps *res;
      res = (struct structBoxProps *)malloc(sizeof(struct structBoxProps));
      res->bp_box_size = $1 ;
      res->bp_box_tex = new Tex();
      res->bp_app = new App();
      
      $$ = res;
    }

spher_props:
   spher_size {
      struct structSpherProps *res;
      res = (struct structSpherProps *)malloc(sizeof(struct structSpherProps));
      res->sp_spher_size = $1 ;
      res->sp_spher_tex = NULL;
      res->sp_app = new App();
      
      $$ = res;
   }
|  spher_size COMMA SPHER_TEX {
      struct structSpherProps *res;
      res = (struct structSpherProps *)malloc(sizeof(struct structSpherProps));
      res->sp_spher_size = $1 ;
      res->sp_spher_tex = $3;
      res->sp_app = new App();
      
      $$ = res;
   }
|  spher_size COMMA app {
      struct structSpherProps *res;
      res = (struct structSpherProps *)malloc(sizeof(struct structSpherProps));
      res->sp_spher_size = $1 ;
      res->sp_spher_tex = NULL;
      res->sp_app = $3;
      
      $$ = res;
   }
|  spher_size COMMA SPHER_TEX COMMA app {
      struct structSpherProps *res;
      res = (struct structSpherProps *)malloc(sizeof(struct structSpherProps));
      res->sp_spher_size = $1 ;
      res->sp_spher_tex = $3;
      res->sp_app = $5;
      
      $$ = res;
   }

/************************ Terminals *************************/

pos_ang: NUMBER NUMBER NUMBER NUMBER NUMBER
{
  float *res = new float[5];
  res[0] = $1; 
  res[1] = $2; 
  res[2] = $3; 
  res[3] = $4; 
  res[4] = $5; 

  $$ = res;
}

box_size: TK_BOX_SIZE NUMBER COMMA NUMBER COMMA NUMBER
{
  $$ = new Vect($2, $4, $6);
}

spher_size: TK_SPHER_SIZE NUMBER
{
  $$ = $2;
}

box_tex:
  box_tex_single { $$ = $1; }
| box_tex COMMA box_tex_single { 
    mix_texture($3, $1);
    delete($3);
    $$ = $1;
}

box_tex_single:
  TEX_XP { $$ = new Tex($1, NULL, NULL, NULL, NULL, NULL); }
| TEX_XN { $$ = new Tex(NULL, $1, NULL, NULL, NULL, NULL); }
| TEX_YP { $$ = new Tex(NULL, NULL, $1, NULL, NULL, NULL); }
| TEX_YN { $$ = new Tex(NULL, NULL, NULL, $1, NULL, NULL); }
| TEX_ZP { $$ = new Tex(NULL, NULL, NULL, NULL, $1, NULL); }
| TEX_ZN { $$ = new Tex(NULL, NULL, NULL, NULL, NULL, $1); }

app:
  app_single { $$ = $1; }
| app COMMA app_single {
    mix_app($3, $1);
    delete($3);
    $$ = $1;
}

app_single:
  TK_AMBIENT COMMA NUMBER COMMA NUMBER {
    Color v($1, $3, $5, 1.0);
    $$ = new App(v);
  }
| TK_DIFFUSE COMMA NUMBER COMMA NUMBER {
    Color v($1, $3, $5, 1.0);
    App *a = new App();
    a->setDiffuse(v);
    $$ = a;
  }
| TK_SHININESS COMMA NUMBER COMMA NUMBER {
    Color v($1, $3, $5, 1.0);
    App *a = new App();
    a->setShininess(v);
    $$ = a;
  }
| TK_SPECULAR COMMA NUMBER COMMA NUMBER {
    Color v($1, $3, $5, 1.0);
    App *a = new App();
    a->setSpecular(v);
    $$ = a;
  }

%%

/*
  Mixe 2 textures : une texture t1 a un seul element, et une texture t2 quelconque.
  ex : t1(a1, NULL, ..., NULL) et t2(b1, ..., b6)
  resultat : t2(a1, b2, ..., b6)
 */
void mix_texture(Tex* t1, Tex* t2)
{
  if (t1->getTex_xp() != NULL) {
    t2->setTex_xp(t1->getTex_xp());
    return ;
  }
  if (t1->getTex_xn() != NULL) {
    t2->setTex_xn(t1->getTex_xn());
    return ;
  }
  if (t1->getTex_yp() != NULL) {
    t2->setTex_yp(t1->getTex_yp());
    return ;
  }
  if (t1->getTex_yn() != NULL) {
    t2->setTex_yn(t1->getTex_yn());
    return ;
  }
  if (t1->getTex_zp() != NULL) {
    t2->setTex_zp(t1->getTex_zp());
    return;
  }
  if (t1->getTex_zn() != NULL) {
    t2->setTex_zn(t1->getTex_zn());
    return ;
  }
  return ;
}

void mix_app(App* a1, App* a2)
{
  if (!(a1->getDiffuse() == a->getDiffuse())) {
    a2->setDiffuse(a1->getDiffuse());
    return ;
  }
  if (!(a1->getAmbient() == a->getAmbient())) {
    a2->setAmbient(a1->getAmbient());
    return ;
  }
  if (!(a1->getSpecular() == a->getSpecular())) {
    a2->setSpecular(a1->getSpecular());
    return ;
  }
  if (!(a1->getShininess() == a->getShininess())) {
    a2->setShininess(a1->getShininess());
    return ;
  }
}

float deg(const float rad)
{
  return (rad * 180.0 / M_PI);
}

void moveCenter(Vect& center, Vect& size)
{
  center += Vect(size[0], -size[1], size[2]);
}

int fileToGroup(FILE *in, Group *gp)
{ 
  a = new App();
  //yydebug = 0;   // debogage (yacc)
  //yy_flex_debug = 0;  // pour le debogage (lex)
  // le pointeur global pr pointe vers gp, donc c'est a *gp qu'on ajoute les objects du fichier
  gr = gp;

  yyin = in;
  yylineno = 0;
  delete(a);
  return yyparse();
}

int yyerror(char *s)
{
  fprintf(stderr, "line %d:%s\n", yylineno, s);
  return 0;
}

