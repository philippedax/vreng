(*s Syntaxe abstraite. *)

type num = Numero of float
type coord = Coord of float * float * float * float | NA
type pointing = POINT
type attribut = string
type position = Position of attribut | PositionNull
type dimension = Dimension of float * float * float
type article = UNDEFINI | DEFINI | DEMONSTRATIF | PRN_INT | PRN_SELECT | ADVERBE

type valeur = 
  | Vbool of bool
  | Vint of int
  | Vfloat of float
  | Vstring of string


type objet = 
  | Objet of article * string
  | ObjetType of string    
  | Ident of string
  | ObjetTrouve of coord * dimension
  | ObjetNull

type ordre = Ordre of attribut

type information = Info of attribut

type demande = Demande of article * attribut

type phrase =
  | Action of ordre * position * objet 
  | Information of information * position * objet    
  | Question of demande * objet 
  | Exclamation of ordre * objet
  | Vide
