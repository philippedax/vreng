(*s 
  Fonction de base pour transferer les donnees depuis C++
  par l'intermediaire de chaines de caracteres.
*)
{
  open Lexing
  open StringParser

  exception String_parsing_failed of (int * string)

}

(*declaration de la forme d'un flottant*)
let num = '-'?['0'-'9']+
let float = num '.' ['0'-'9']* (['e''E']['+''-']num)?
let blank = [' ''\n''\t'',']
(* nous ne tenons pas compte des tabulations '\t' et des sauts de ligne :'\n' *)

(*on oblige a ce que les valeurs soient tous des flottants*)
rule stringToPosDim = parse
  | blank { stringToPosDim lexbuf }
  | float { VFLOAT(float_of_string(lexeme lexbuf)) }
  | num   { VFLOAT(float_of_int(int_of_string(lexeme lexbuf))) }
  | eof   { EOF }
  | _     { raise (String_parsing_failed(lexeme_start lexbuf,lexeme lexbuf) )}
  
