type token =
  | EOF
  | VFLOAT of (float)

val stringdepart :
  (Lexing.lexbuf  -> token) -> Lexing.lexbuf -> RequeteSyntax.objet
