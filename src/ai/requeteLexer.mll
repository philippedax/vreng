(*s Analyse lexical des fichiers*)
{
  open Lexing
  open RequeteParser

 (*s Exceptions pouvant être levées pendant l'analyse lexicale. *)
  exception Illegal_character of (int * string)
  exception Illegal_syntax of (int * int)
}

(*s Expressions régulières. *)

let alpha = ['a'-'z''A'-'Z']
let mot = alpha (alpha | '-')*
let ident = alpha (alpha | ['0'-'9''_''-'])*
(* les identificateurs doivent obligatoirement debute par un caractère *)
(* et peut ensuite être suivis d'un chiffre ou d'un caractère ou d'un "_".*)
(* nous ne ferons la reconnaissance des idents que sur la fin pour pouvoir *)  
(* reconnaitre les autres tokens. *)

let blank = [' ''\n''\t']
(* nous ne tenons pas compte des tabulations '\t' et des saut de ligne :'\n' *)


(*let num = '-'?['0'-'9']+*)
(*let coordonnees = num','num','num*)
(* les constatantes sont pour nous une suite de chiffre et *)
(* nous autorisons les  -"0000" *)

(*
let float = num '.' ['0'-'9']* (['e''E']['+''-']num)?
*)

(*s les différents articles. *)
let articleNotFound     = ("son"|"sa"|"mon"|"ma"|"ton"|"ta"|"par")
let articleDefinis      = ("le"|"la"|"les")
let articleIndefinis    = ("du"|"des"|"de"|"une"|"un"|"au")
let articleDemonstratif = ("ce"|"cette"|"cet"|"ces")
let adverbeInterrogatif = ("quand"|"comment"|"où"|"pourquoi"|"que"|"combien")
let pronomInterrogatif  = ("qui"|"ce qui"|"quoi"|"lequel")
let pronomSelection     = ("tout"|"tous"|"toute"|"toutes")

(*s les fonctions *) 
let actions_exclamation = ("va"|"pars"|"prend"|"deplace"|"avance")
let actions_exclamation_functions = 
  ("manipule"|"navigue"|"ouvre"|"ferme"|"deverrouiller"
  |"verouille"|"va"|"panier"|"pousse"|"tire"|"frappe"
  |"prend"|"lache"|"tourne"|"detruit"|"entre"|"balle"
  |"connecte"|"deconnecte"|"dessin"|"sort"|"regarde"
  |"imprime"|"joue"|"arrete"|"pause"|"continue"
  |"boucle"|"rembobine"|"telecharge"|"allume"|"eteint"
  |"accelere"|"augmentel'amplitude"|"diminuel'amplitude"
  |"augmentelafrequence"|"diminuelafrequence"
  |"augmentelaphase"|"diminuelaphase"|"augmentelatransparence"
  |"diminuelatransparence"|"augmentelebrouillard"
  |"dominuelebrouillard"|"raz"|"anime"|"marche"
  |"approche"|"suivant"|"precedent"|"quitte"
  |"retire"|"monte"|"descend"|"appelle")

let actions_interrogatif= ("vois-je"|"sais-je") 

(*s les positions *)
let position = 
  ("devant"|"dessous"|"dessus"
  |"au-dessus"|"au-dessous"
  |"derriere"|"sous"|"sur"
  |"a cote"|"a gauche"
  |"a droite"|"en bas"
  |"en haut"|"vers"|"dans"
  |"dehors"|"a l\'interieur"
  |"a l\'exterieur")

(*s Point d'entrée du lexeur. *)
rule phraseToToken = parse
  | blank { phraseToToken lexbuf }
(*  | num { VINT(int_of_string(lexeme lexbuf)) }*)
(*  | float { VFLOAT(float_of_string(lexeme lexbuf)) }*)
  | "!" { EXCLAMATION }
  | "." { INFORMATION }
  | "?" { INTERROGATION }
  | "oui"    { OUI }
  | "non"    { NON }
  | position { VPOSITION(lexeme lexbuf) }
  | actions_exclamation_functions { VACTIONS_ORDRE(lexeme lexbuf) }
  | actions_exclamation { VACTIONS_ORDRE(lexeme lexbuf) }
  | actions_interrogatif{ VQUESTION(lexeme lexbuf)}
  | articleDefinis      { phraseToToken lexbuf }
  | articleIndefinis    { ART_UNDEF }
  | articleDemonstratif { ART_DEMONSTRATIF }
  | adverbeInterrogatif { ADV_INTERROGATIF }
  | pronomInterrogatif  { PRN_INTERROGATIF }
  | pronomSelection     { PRN_SELECTION }
  | mot   { VMOT(lexeme lexbuf) }
  | ident { VIDENT(lexeme lexbuf) }
  | eof   { EOF }
  | _     { raise (Illegal_character(lexeme_start lexbuf,lexeme lexbuf)) }
  
  
  (*s Contraintes :
	  - les mots cles sont toujours en minuscule.
	  - les mots ne peuvent pas avoir d'accents.*)
