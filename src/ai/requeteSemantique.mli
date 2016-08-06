open Printf
open RequeteSyntax

exception Semantique_error of string;;

val dcptCoord            : string -> int;;
val analyse_sem          : phrase -> int;;
val analyse_sem_action   : phrase -> int;;
