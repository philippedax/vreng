open Printf
open RequeteSyntax

(*s creation de l'erreur semantique, concerne surtout le fait de pas
  avoir pu trouver d'objets*)
exception Semantique_error of string;;

(************* declarations fonctions externes **************)


(*s a partir d'un nom, nous cherchons un type défini*)
external rechercheNom : string -> string =
    "recherche_Objet";;

(*s a partir d'une demande de désignation suivant un type, on déplace
  l'utilisateur à la positoin voulue.*)
external rechercheClickType : string -> string =
    "recherche_Type";;

(*s a partir d'une demande de désignation pour executer un l'ordre *)
external rechercheClickTypeGen : string -> string -> int =
    "recherche_Typegen";;

(*s recherche des fonctions d'un objet, pour un acces à un objet
  par sa fonctionnalité *)
external rechercheFunction : string -> string -> int = 
    "recherche_Func";;

(*s on demande simplement un déplacement suivant une position voulue*)
external deplacementUser : float -> float -> float -> float -> int -> int =
    "deplacement_to_Objet";;

(*s on demande un déplacement vers un object et suivant un positionnement *)
external dcptProximite : string -> string -> int =
    "deplacement_to_Proximite";;

(*s fonction pour lister les objets visible *)
external listerObjets : string -> int = 
    "viewed_objects";;

(*s fonction de débuggage pour afficher des infos*)
external debug : string -> unit =
    "msg_debug";;


(************* declarations fonctions **************)
let actionsTranslation = [
  ("manipule","Manip");
  ("navigue","Navig");
  ("ouvre","Open");
  ("ferme","Close");
  ("deverrouiller","Unlock");
  ("verouille","Lock");
  ("va","Approach");
  ("panier","Basket");
  ("pousse","Push");
  ("tire","Pull");
  ("frappe","Shoot");
  ("prend","Take");
  ("lache","Drop");
  ("tourne","Turn");
  ("detruit","Destroy");
  ("entre","Enter");
  ("balle","Ball");
  ("connecte","Connect");
  ("deconnecte","Disconnect");
  ("dessin","Draw");
  ("","Import");
  ("sort","Exit");
  ("regarde","View");
  ("imprime","Print");
  ("joue","Play");
  ("arrete","Stop");
  ("pause","Pause");
  ("continue","Continue");
  ("boucle","Loop");
  ("rembobine","Rewind");
  ("telecharge","Download");
  ("allume","On");
  ("eteint","Off");
  ("accelere","Accel");
  ("augmente l'amplitude","Ampl+");
  ("diminue l'amplitude","Ampl-");
  ("augmente la frequence","Freq+");
  ("diminue la frequence","Freq-");
  ("augmente la Phase","Phase+");
  ("diminue la Phase","Phase-");
  ("augmente la transparence","Transp+");
  ("diminue la transparence","Transp-");
  ("augmente le brouillard","Fog+");
  ("dominue le brouillard","Fog-");
  ("raz","Defaults");
  ("","Still");
  ("anime","Anim");
  ("marche","Walk");
  ("","RotZ+");
  ("","RotZ-");
  ("","RotX+");
  ("","RotX-");
  ("","RotY+");
  ("","RotY-");
  ("","Focus");
  ("","Navigate");
  ("","Hi");
  ("","Bye");
  ("","Ask");
  ("","Sit");
  ("","Reset");
  ("","Nack");
  ("","Clap");
  ("","New");
  ("","Yes");
  ("","No");
  ("","Eyes");
  ("","Joy");
  ("","Sad");
  ("","Surp");
  ("","Jag");
  ("","Test");
  ("","Listen");
  ("","EyeL");
  ("","Mouth");
  ("approche","Approach");
  ("suivant","Next");
  ("precedent","Prev");
  ("","P+10");
  ("","P-10");
  ("","LookLeft");
  ("","LookRight");
  ("sauvegarde","Save");
  ("","Pin");
  ("quitte","Leave");
  ("","Carry");
  ("retire","Remove");
  ("","Roll");
  ("monte","Up");
  ("descend","Down");
  ("appelle","Call")
];;

let objetsTranslation= [
  ("porte","door");
  ("portes","door");
  ("mirroir","mirror");
  ("mirroirs","mirror");
  ("portail","gate");
  ("portails","gate");
  ("sol","ground");
  ("sols","ground");
  ("lumiere","light");
  ("lumieres","light");
  ("mirage","mirage");
  ("mirages","mirage");
  ("modele","model");
  ("modeles","model");
  ("androide","android");
  ("androides","android");
  ("zone","area");
  ("zones","area");
  ("tableau","board");
  ("tableaux","board");
  ("livre","book");
  ("livres","book");
  ("bouton","button");
  ("boutons","button");
  ("chaudron","cauldron");
  ("chaudrons","cauldron");
  ("clip","clip");
  ("clips","clip");
  ("horloge","clock");
  ("horloges","clock");
  ("document","doc");
  ("documents","doc");
  ("bonhomme","guy");
  ("bonhommes","guy");
  ("machine","host");
  ("machines","host");
  ("ascenseur","lift");
  ("ascenseurs","lift");
  ("film","movie");
  ("films","movie");
  ("musique","music");
  ("musiques","music");
  ("particule","particle");
  ("particules","particle");
  ("surface","plane");
  ("surfaces","plane");
  ("etoile","stars");
  ("etoiles","stars");
  ("marche","step");
  ("marches","step");
  ("texte","text");
  ("textes","text");
  ("chose","thing");
  ("choses","thing");
  ("mur","wall");
  ("murs","wall");
  ("eau","water");
  ("eaux","water");
  ("lien","web");
  ("liens","web")
];;


(*s fonction auxiliaire qui transforme la position et dim en 
  objet et permet de deplacer l'utilsateur a la position voulue*)
let dcptCoord posfound =  
  try
    let objt = StringParser.stringdepart StringLexer.stringToPosDim(Lexing.from_string posfound)
    in
    match objt with 
	ObjetTrouve(Coord(-1.0,-1.0,-1.0,-1.0),Dimension(-1.0,-1.0,-1.0)) ->  raise (Semantique_error "objet non trouve : recherché et non trouvé")
      | ObjetTrouve(Coord(cx,cy,cz,cax),Dimension(dx,dy,dz)) -> (deplacementUser cx cy cz cax 1)
      | _ -> -99
  with _ -> raise (Semantique_error "objet non trouve mais pas normal")
    (* TODO :il peut pas arrive la !! comment il fait ????? :oooo beans and camlinit.cc*)


(*s On analyse ici l'action, pour le moment nous ne traitons que les
  cas ou on a des ordres.*)
let analyse_sem_action action =
    match action with 
	Exclamation (Ordre(o),ObjetNull) -> 
	  let trouve = List.assoc o actionsTranslation
	  in let alors = (rechercheFunction trouve "NA")
	  in alors

      |  Exclamation (Ordre(o),Objet(PRN_SELECT, objtype)) -> 
	  let trouve = List.assoc o actionsTranslation
	  in let objtrouve = List.assoc objtype objetsTranslation
	  in (rechercheFunction trouve objtrouve)

      | Exclamation (Ordre(o),Objet(DEMONSTRATIF,obj)) -> 
	  let trouve = List.assoc o actionsTranslation
	  in let objtrouve = List.assoc obj objetsTranslation
	  in rechercheClickTypeGen objtrouve trouve
	  

      | Action (Ordre(o),Position(p),Ident(obj)) -> 
	  let positiontrouve = rechercheNom obj 
	  in (dcptCoord positiontrouve)

      | Action (Ordre(o),Position(p),Objet(UNDEFINI,obj)) -> 
	  (dcptProximite obj p)

      (*TODO : tenir compte de le position en fonction des dimension de l'objet*)
      | Action (Ordre(o),Position(p),Objet(DEFINI,obj)) -> 
	  let positiontrouve = rechercheNom obj 
	  in  (dcptCoord positiontrouve)

      | Action (Ordre(o),Position(p),Objet(DEMONSTRATIF,obj)) -> 
	  let positiontrouve = rechercheClickType obj
	  in 
	  if ((String.length positiontrouve) < 7)
	  then 1
	  else dcptCoord positiontrouve

      | Action (Ordre(o),PositionNull,ObjetNull) -> 
	  let trouve = List.assoc o actionsTranslation
	  in (rechercheFunction trouve "NA")

      | Action (Ordre(o),PositionNull,Objet(PRN_SELECT, objtype)) -> 
	  let trouve = List.assoc o actionsTranslation
	  in let objtrouve = List.assoc objtype objetsTranslation
	  in (rechercheFunction trouve objtrouve)
	       
      | Question(Demande(ADVERBE,"vois-je"),Objet(UNDEFINI,obj)) -> let _ = listerObjets obj
	in 0
	     (*TODO : Traitement des questions !*)
      | _ -> -98
;;

(*s On analyse l'abre semantique ainsi crée*)
let analyse_sem arbre =
  match arbre with 
    | Vide                 -> raise (Semantique_error "requete vide : bug ?")
    | Action (a,b,c)       -> analyse_sem_action arbre
    | Question (dem,obj)   -> analyse_sem_action arbre
    | Exclamation (a,b)    -> analyse_sem_action arbre
    | Information (a,b,c)  -> analyse_sem_action arbre
    	
;;



