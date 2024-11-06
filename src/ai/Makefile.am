AUTOMAKE_OPTIONS = foreign no-dependencies

AI_INIT = ai.cpp dummy_.ml
AI_SRCS = $(AI_INIT) stringParser.mly stringLexer.mll requeteParser.mly requeteLexer.mll requeteAffichage.ml requeteSemantique.ml analyseur.ml
AI_HDRS = ai.hpp analyseur.mli requeteAffichage.mli stringLexer.mli requeteParser.mli requeteLexer.mli requeteSemantique.mli requeteSyntax.mli

EXTRA_DIST = $(AI_SRCS) $(AI_HDRS)

GEN = stringLexer.ml stringParser.mli stringParser.ml requeteLexer.ml requeteParser.mli requeteParser.ml
CML = stringParser.ml stringLexer.ml requeteLexer.ml requeteParser.ml requeteAffichage.ml requeteSemantique.ml analyseur.ml
CMLI = requeteSyntax.mli stringParser.mli stringLexer.mli requeteParser.mli requeteLexer.mli requeteAffichage.mli requeteSemantique.mli analyseur.mli
CMI = $(CMLI:.mli=.cmi)
CMO = $(CML:.ml=.cmo)
CMX = $(CML:.ml=.cmx)

SRC = $(top_srcdir)/src
CFLAGS = @OCAML_CFLAGS@ @XML2_CFLAGS@ @UBIT_CFLAGS@
CXXFLAGS += -Wno-writable-strings

AM_CPPFLAGS = $(CFLAGS) -I$(SRC)/base -I$(SRC)/net -I$(SRC)/world -I$(SRC)/3d -I$(SRC)/gui

if ENABLE_OCAML

libai_a_SOURCES = $(AI_INIT) ai.hpp
noinst_LIBRARIES = libai.a

SUFFIXES: .o .c .cmo .cmi .cmx .ml .mli .mll .mly

.ml.o:
	ocamlopt -c $<
.mli.cmi:
	ocamlc -c $<
.cmo:
	ocamlc -c $<
.mll.ml:
	ocamllex $<
.mly.ml:
	ocamlyacc $<
.mly.mli:
	ocamlyacc $<

all: dummy_.o

dummy_.o: $(GEN) $(CMI) $(CML) dummy_.ml
	ocamlopt -output-obj $(CML) -o dummy.o
	mv dummy.o dummy_.o

endif

clean-generic:
	-@rm -f $(GEN) $(CMO) $(CMI) $(CMX) dummy_.cmx dummy_.cmi Libai.a
