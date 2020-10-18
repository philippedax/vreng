open Printf
open RequeteSyntax

exception Not_found;;


(*s cette fonction permet l'affichage de tout l'arbre *)

let affichage_coord coord =
  match coord with
    | Coord(x,y,z,ax) ->  "les coordonnees de l'objet voulu sont :"^(string_of_float x)^"y: "^(string_of_float y)^" z: "^(string_of_float z)^" ax: "^(string_of_float ax)^"."
    | NA -> "coordonnees non trouvé"

let affichage_valeur lavaleur =
  match lavaleur with 
    | Vbool(true) -> "oui"
    | Vbool(false) -> "non"
    | Vint(i) -> string_of_int i
    | Vfloat(ff) -> string_of_float ff
    | Vstring(s) -> s


let affichage_objets obj =
  match obj with 
    | Objet (UNDEFINI,ob) -> "Objet(UNDEF,"^ob^" )"
    | Objet (DEFINI,ob) -> "Objet(DEF,"^ob^" )"
    | Objet (DEMONSTRATIF,ob) -> "Objet(DEMONS,"^ob^" )"
    | Objet (PRN_INT,ob) -> "Objet(PRN_INT,"^ob^" )"
    | Objet (PRN_SELECT,ob) -> "Objet(PRN_SELECT,"^ob^" )"
    | Objet (ADVERBE,ob) -> "Objet(ADVERBE,"^ob^" )"
    | Ident (ad) -> "Ident( "^ad^" )"
    | ObjetTrouve (Coord(x,y,z,ax),Dimension(dx,dy,dz)) -> "Object(Coord("^( string_of_float x)^","^( string_of_float y)^","^( string_of_float z)^" ),Dimension("^","^( string_of_float dx)^","^( string_of_float dy)^","^( string_of_float dz)^"))"
    | ObjetTrouve (NA,Dimension(dx,dy,dz)) -> "Object( N/A ),Dimension("^( string_of_float dx)^","^( string_of_float dy)^","^( string_of_float dz)^"))"
    | ObjetNull -> "ObjetNull"
    | ObjetType(ob) -> "ObjetType("^ob^" )"

let affichage_position position =
  match position with 
    | Position (o) -> "Position( "^o^" )"
    | PositionNull -> "PositionNull"

let affichage_demande demande =
  match demande with 
    |Demande (art,dem) -> "Demande( ARTICLE,"^dem^" )"

let affichage_ordre ordre =
  match ordre with 
    |Ordre (o) -> "Ordre( "^o^" )"


let affichage_information ordre =
  match ordre with 
    |Info (o) -> "information( "^o^" )"



let affichage_arbre arbre =
  match arbre with 
    | Action (a,b,c) -> "Action( "^affichage_ordre(a)^" ,
"^affichage_position(b)^" , "^affichage_objets(c)^" )\n"
    | Exclamation (a,b) ->  "Exclamation( "^affichage_ordre(a)
 	^" , "^affichage_objets(b)^" )\n"
    | Information (a,b,c) -> "Information( "^affichage_information(a)^" ,
"^affichage_position(b)^" , "^affichage_objets(c)^" )\n"
    | Question (dem,obj) -> "Question( "^affichage_demande(dem)^","^affichage_objets(obj)^" )\n"
    | Vide -> "\nNous n'avons pas de requete a analyser token <Vide> recu\n"
	  
	  
