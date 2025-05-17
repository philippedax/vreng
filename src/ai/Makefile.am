AUTOMAKE_OPTIONS = foreign no-dependencies

INIT = ai.cpp dummy_.ml
SRCS = $(INIT) stringParser.mly stringLexer.mll requeteParser.mly requeteLexer.mll requeteAffichage.ml requeteSemantique.ml analyseur.ml
HDRS = ai.hpp analyseur.mli requeteAffichage.mli stringLexer.mli requeteParser.mli requeteLexer.mli requeteSemantique.mli requeteSyntax.mli

EXTRA_DIST = $(SRCS) $(HDRS)

ML  = stringLexer.ml stringParser.mli stringParser.ml requeteLexer.ml requeteParser.mli requeteParser.ml
CML = stringParser.ml stringLexer.ml requeteLexer.ml requeteParser.ml requeteAffichage.ml requeteSemantique.ml analyseur.ml
MLI = requeteSyntax.mli stringParser.mli stringLexer.mli requeteParser.mli requeteLexer.mli requeteAffichage.mli requeteSemantique.mli analyseur.mli
#CMI = $(MLI:.mli=.cmi)
#CMO = $(CML:.ml=.cmo)
#CMX = $(CML:.ml=.cmx)

CFLAGS = @OCAML_CFLAGS@ @UBIT_CFLAGS@ @XML2_CFLAGS@
CXXFLAGS += -Wno-c++11-compat-deprecated-writable-strings
SRC = $(top_srcdir)/src
AM_CPPFLAGS = $(CFLAGS) -I$(SRC)/base -I$(SRC)/net -I$(SRC)/world -I$(SRC)/3d -I$(SRC)/gui

if ENABLE_OCAML
libai_a_SOURCES = $(INIT) ai.hpp
noinst_LIBRARIES = libai.a

SUFFIXES: .o .c .cmo .cmi .cmx .ml .mli .mll .mly
.ml.o:    ; ocamlopt -c $<
.mli.cmi: ; ocamlc -c $<
.cmo:     ; ocamlc -c $<
.mll.ml:  ; ocamllex $<
.mly.ml:  ; ocamlyacc $<
.mly.mli: ; ocamlyacc $<

all: dummy_.o
dummy_.o: $(ML) $(CMI) $(CML) dummy_.ml
	ocamlopt -output-obj $(CML) -o dummy_.o
endif
#clean-generic: ; -@rm -f $(ML) $(CMO) $(CMI) $(CMX) dummy_.cmx dummy_.cmi Libai.a
