type token =
  | EOF
  | VINT of (int)
  | VFLOAT of (float)
  | VIDENT of (string)
  | VMOT of (string)
  | VACTIONS_ORDRE of (string)
  | VQUESTION of (string)
  | VPOSITION of (string)
  | OUI
  | NON
  | EXCLAMATION
  | INFORMATION
  | INTERROGATION
  | ART_DEMONSTRATIF
  | ART_UNDEF
  | PRN_INTERROGATIF
  | ADV_INTERROGATIF
  | PRN_SELECTION

open Parsing;;
let _ = parse_error;;
# 4 "requeteParser.mly"
  open RequeteSyntax
  open RequeteLexer
    
  exception Illegal_syntax of (int * int)

  let debut_symbol = ref 0 
  let fin_symbol = ref 0

  let cur_loc() = symbol_start(), symbol_end()
    (*
      l p _ = r(P.P)
    *)

  let parse_error _ = raise(RequeteLexer.Illegal_syntax ( symbol_start(), symbol_end()))

# 40 "requeteParser.ml"
let yytransl_const = [|
    0 (* EOF *);
  264 (* OUI *);
  265 (* NON *);
  266 (* EXCLAMATION *);
  267 (* INFORMATION *);
  268 (* INTERROGATION *);
  269 (* ART_DEMONSTRATIF *);
  270 (* ART_UNDEF *);
  271 (* PRN_INTERROGATIF *);
  272 (* ADV_INTERROGATIF *);
  273 (* PRN_SELECTION *);
    0|]

let yytransl_block = [|
  257 (* VINT *);
  258 (* VFLOAT *);
  259 (* VIDENT *);
  260 (* VMOT *);
  261 (* VACTIONS_ORDRE *);
  262 (* VQUESTION *);
  263 (* VPOSITION *);
    0|]

let yylhs = "\255\255\
\001\000\001\000\001\000\001\000\001\000\002\000\005\000\005\000\
\003\000\004\000\004\000\004\000\004\000\004\000\006\000\006\000\
\000\000"

let yylen = "\002\000\
\004\000\003\000\002\000\003\000\001\000\001\000\002\000\002\000\
\001\000\001\000\002\000\002\000\001\000\001\000\002\000\002\000\
\002\000"

let yydefred = "\000\000\
\000\000\000\000\005\000\006\000\000\000\000\000\017\000\000\000\
\000\000\007\000\008\000\014\000\013\000\009\000\003\000\000\000\
\000\000\000\000\000\000\000\000\010\000\000\000\012\000\011\000\
\015\000\016\000\000\000\002\000\004\000\001\000"

let yydgoto = "\002\000\
\007\000\008\000\019\000\020\000\009\000\021\000"

let yysindex = "\255\255\
\001\000\000\000\000\000\000\000\002\255\005\255\000\000\255\254\
\249\254\000\000\000\000\000\000\000\000\000\000\000\000\016\255\
\013\255\017\255\001\255\012\255\000\000\011\255\000\000\000\000\
\000\000\000\000\014\255\000\000\000\000\000\000"

let yyrindex = "\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000"

let yygindex = "\000\000\
\000\000\000\000\000\000\005\000\000\000\017\000"

let yytablesize = 273
let yytable = "\001\000\
\003\000\012\000\013\000\012\000\013\000\014\000\017\000\010\000\
\015\000\018\000\011\000\016\000\017\000\016\000\017\000\018\000\
\025\000\018\000\023\000\024\000\026\000\028\000\029\000\027\000\
\030\000\022\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\004\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\005\000\
\006\000"

let yycheck = "\001\000\
\000\000\003\001\004\001\003\001\004\001\007\001\014\001\006\001\
\010\001\017\001\006\001\013\001\014\001\013\001\014\001\017\001\
\004\001\017\001\003\001\004\001\004\001\010\001\012\001\019\000\
\011\001\009\000\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\005\001\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\015\001\
\016\001"

let yynames_const = "\
  EOF\000\
  OUI\000\
  NON\000\
  EXCLAMATION\000\
  INFORMATION\000\
  INTERROGATION\000\
  ART_DEMONSTRATIF\000\
  ART_UNDEF\000\
  PRN_INTERROGATIF\000\
  ADV_INTERROGATIF\000\
  PRN_SELECTION\000\
  "

let yynames_block = "\
  VINT\000\
  VFLOAT\000\
  VIDENT\000\
  VMOT\000\
  VACTIONS_ORDRE\000\
  VQUESTION\000\
  VPOSITION\000\
  "

let yyact = [|
  (fun _ -> failwith "parser")
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 3 : 'verbe) in
    let _2 = (Parsing.peek_val __caml_parser_env 2 : 'position) in
    let _3 = (Parsing.peek_val __caml_parser_env 1 : 'objet) in
    Obj.repr(
# 46 "requeteParser.mly"
                                    ( Action(_1,_2,_3) )
# 207 "requeteParser.ml"
               : RequeteSyntax.phrase))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : 'verbe) in
    let _2 = (Parsing.peek_val __caml_parser_env 1 : 'objet) in
    Obj.repr(
# 47 "requeteParser.mly"
                                    ( Exclamation(_1, _2) )
# 215 "requeteParser.ml"
               : RequeteSyntax.phrase))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 1 : 'verbe) in
    Obj.repr(
# 48 "requeteParser.mly"
                                    ( Exclamation(_1,ObjetNull) )
# 222 "requeteParser.ml"
               : RequeteSyntax.phrase))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 2 : 'question) in
    let _2 = (Parsing.peek_val __caml_parser_env 1 : 'objetundef) in
    Obj.repr(
# 49 "requeteParser.mly"
                                    ( Question(_1,_2) )
# 230 "requeteParser.ml"
               : RequeteSyntax.phrase))
; (fun __caml_parser_env ->
    Obj.repr(
# 50 "requeteParser.mly"
      ( Vide )
# 236 "requeteParser.ml"
               : RequeteSyntax.phrase))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 0 : string) in
    Obj.repr(
# 54 "requeteParser.mly"
               ( Ordre(_1) )
# 243 "requeteParser.ml"
               : 'verbe))
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 0 : string) in
    Obj.repr(
# 58 "requeteParser.mly"
                             ( Demande(PRN_INT,_2) )
# 250 "requeteParser.ml"
               : 'question))
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 0 : string) in
    Obj.repr(
# 59 "requeteParser.mly"
                             ( Demande(ADVERBE,_2) )
# 257 "requeteParser.ml"
               : 'question))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 0 : string) in
    Obj.repr(
# 63 "requeteParser.mly"
          ( Position(_1) )
# 264 "requeteParser.ml"
               : 'position))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 0 : 'objetundef) in
    Obj.repr(
# 67 "requeteParser.mly"
             (_1)
# 271 "requeteParser.ml"
               : 'objet))
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 0 : string) in
    Obj.repr(
# 68 "requeteParser.mly"
                        ( Objet(DEMONSTRATIF,_2) )
# 278 "requeteParser.ml"
               : 'objet))
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 0 : string) in
    Obj.repr(
# 69 "requeteParser.mly"
                          ( Ident(_2) )
# 285 "requeteParser.ml"
               : 'objet))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 0 : string) in
    Obj.repr(
# 70 "requeteParser.mly"
       ( Objet(DEFINI,_1) )
# 292 "requeteParser.ml"
               : 'objet))
; (fun __caml_parser_env ->
    let _1 = (Parsing.peek_val __caml_parser_env 0 : string) in
    Obj.repr(
# 71 "requeteParser.mly"
         ( Ident(_1) )
# 299 "requeteParser.ml"
               : 'objet))
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 0 : string) in
    Obj.repr(
# 75 "requeteParser.mly"
                ( Objet(UNDEFINI,_2) )
# 306 "requeteParser.ml"
               : 'objetundef))
; (fun __caml_parser_env ->
    let _2 = (Parsing.peek_val __caml_parser_env 0 : string) in
    Obj.repr(
# 76 "requeteParser.mly"
                    ( Objet(PRN_SELECT,_2) )
# 313 "requeteParser.ml"
               : 'objetundef))
(* Entry program *)
; (fun __caml_parser_env -> raise (Parsing.YYexit (Parsing.peek_val __caml_parser_env 0)))
|]
let yytables =
  { Parsing.actions=yyact;
    Parsing.transl_const=yytransl_const;
    Parsing.transl_block=yytransl_block;
    Parsing.lhs=yylhs;
    Parsing.len=yylen;
    Parsing.defred=yydefred;
    Parsing.dgoto=yydgoto;
    Parsing.sindex=yysindex;
    Parsing.rindex=yyrindex;
    Parsing.gindex=yygindex;
    Parsing.tablesize=yytablesize;
    Parsing.table=yytable;
    Parsing.check=yycheck;
    Parsing.error_function=parse_error;
    Parsing.names_const=yynames_const;
    Parsing.names_block=yynames_block }
let program (lexfun : Lexing.lexbuf -> token) (lexbuf : Lexing.lexbuf) =
   (Parsing.yyparse yytables 1 lexfun lexbuf : RequeteSyntax.phrase)
