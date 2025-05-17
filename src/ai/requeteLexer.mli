(*s
token buf retourne a chaque appel de buf le token associe.
Il leve l'exception Erreur_lex si il n'arrive pas a trouve de token
associe.
*)

exception Illegal_character of (int * string)
exception Illegal_syntax of (int * int)

val phraseToToken  : Lexing.lexbuf -> RequeteParser.token;;
