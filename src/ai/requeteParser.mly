/*s Analyse syntaxique. */

%{
  open RequeteSyntax
  open RequeteLexer
    
  exception Illegal_syntax of (int * int)

  let debut_symbol = ref 0 
  let fin_symbol = ref 0

  let cur_loc() = symbol_start(), symbol_end()
    (*
      let parse_error _ = raise(Parsing.Parse_error)
    *)

  let parse_error _ = raise(RequeteLexer.Illegal_syntax ( symbol_start(), symbol_end()))

%}

/*s Declaration des tokens. */

%token EOF 
%token <int> VINT
%token <float> VFLOAT
%token <string> VIDENT
%token <string> VMOT
%token <string> VACTIONS_ORDRE
%token <string> VQUESTION
%token <string> VPOSITION
%token OUI NON 
%token EXCLAMATION INFORMATION INTERROGATION
%token ART_DEMONSTRATIF ART_UNDEF
%token PRN_INTERROGATIF ADV_INTERROGATIF PRN_SELECTION

/*s Point d'entree de la grammaire. */

%type <RequeteSyntax.phrase> program
%start program

%%

/*s Regles de grammaire. */

program :
| verbe position objet INFORMATION  { Action($1,$2,$3) }
| verbe objet EXCLAMATION           { Exclamation($1, $2) }
| verbe EXCLAMATION                 { Exclamation($1,ObjetNull) }
| question objetundef INTERROGATION { Question($1,$2) }
| EOF { Vide }
;

verbe:
VACTIONS_ORDRE { Ordre($1) }
;

question:
| PRN_INTERROGATIF VQUESTION { Demande(PRN_INT,$2) }
| ADV_INTERROGATIF VQUESTION { Demande(ADVERBE,$2) }
;

position:
VPOSITION { Position($1) }
;

objet:
| objetundef {$1}
| ART_DEMONSTRATIF VMOT { Objet(DEMONSTRATIF,$2) }
| ART_DEMONSTRATIF VIDENT { Ident($2) }
| VMOT { Objet(DEFINI,$1) }
| VIDENT { Ident($1) }
;

objetundef:
| ART_UNDEF VMOT{ Objet(UNDEFINI,$2) }
| PRN_SELECTION VMOT{ Objet(PRN_SELECT,$2) }
;
