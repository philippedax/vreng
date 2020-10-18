open Printf 
open RequeteLexer
open RequeteParser
open RequeteAffichage 
open RequeteSemantique

external information : string -> unit = 
    "msg_info";;

let parse lb =
    let a = RequeteParser.program RequeteLexer.phraseToToken lb in
    Parsing.clear_parser ();
    a
;;

let process_lex_yacc entree =
    let lb1 = Lexing.from_string entree
    in let arbre = parse lb1
    in arbre
;;

let process_semantique arbresemantik =
  analyse_sem arbresemantik 
;;

(*
  Les valeurs de retour sont simple : 

  -> 99 a 90 : bug

  ->  1 : requiere 1 selection de souris.

  ->  0 : tout est ok .

  -> -1 : erreur lexicale .
  -> -2 : erreur syntaxique
  -> -3 : erreur semantique : objet inexistant ou autre.
  -> -4 : erreur Interne (pbs XML et autre)

  -> -90 a -99 : bug
*)


let entree_analyse entree = 
  try 
    let ab = process_lex_yacc (entree)
      (*in  process_semantique ab;affichage_arbre ab;*)
    in (* information (affichage_arbre ab); *)
    let retDcpt = (process_semantique ab)
    in retDcpt
(*
    if (retDcpt = 0)
    then  information ("Requete Acceptée"); 0 (* 0 tout est ok *)
    else if (retDcpt == -1) (* erreur XML *)
    then information ("Erreur XML ou autres"); -4 (*erreur XML*)
    else if (retDcpt == -2) (*erreur rien trouvé*)
    then (*information ("Erreur Semantique : Element non trouvé");*)
      -3
    else (information ("Erreur Inconnue ou non traitée code erreur :"^(string_of_int retDcpt))); -90 
*)  
  with  
      RequeteLexer.Illegal_character(n,c) 
      -> information ("Erreur Lexicale:\""
      ^c
      ^"\" non compris:"^(string_of_int n)^"eme caractere ");
	-1

    | RequeteLexer.Illegal_syntax(count1,count2)
      -> information ("Erreur syntaxique debut : "
      ^(string_of_int count1)
      ^", fin : "
      ^(string_of_int count2)); 
	-2

    | RequeteSemantique.Semantique_error(c) 
      -> information ("Erreur Semantique:\""^c^"\"");
	-3
;;


Callback.register "caml_process_lex_yacc" process_lex_yacc;
Callback.register "caml_entree_analyse" entree_analyse;;
