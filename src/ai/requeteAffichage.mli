open Printf
open RequeteSyntax

(*s Cette interface nous permet d'afficher les 
  structures et ainsi il nous permet de debugger.
*)

exception Not_found;;

val affichage_valeur    : valeur   -> string
val affichage_objets    : objet    -> string 
val affichage_position  : position -> string
val affichage_ordre     : ordre    -> string
val affichage_arbre     : phrase   -> string
