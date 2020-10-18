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

val program :
  (Lexing.lexbuf  -> token) -> Lexing.lexbuf -> RequeteSyntax.phrase
