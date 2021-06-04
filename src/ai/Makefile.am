AUTOMAKE_OPTIONS = foreign

if OCAML

OCAML_INIT = aiinit.cpp dummy_.ml

OCAML_SRCS = $(OCAML_INIT) stringParser.mly stringLexer.mll requeteParser.mly requeteLexer.mll requeteAffichage.ml requeteSemantique.ml analyseur.ml

OCAML_HDRS = aiinit.hpp analyseur.mli requeteAffichage.mli stringParser.mli stringLexer.mli requeteParser.mli requeteLexer.mli requeteSemantique.mli requeteSyntax.mli

EXTRA_DIST = $(OCAML_SRCS) $(OCAML_HDRS)

AM_CPPFLAGS = @OCAML_CFLAGS@ @XML2_CFLAGS@ @UBIT_CFLAGS@ -I$(top_srcdir)/src/sys -I$(top_srcdir)/src/vreng -I$(top_srcdir)/src/com -I$(top_srcdir)/src/net -I$(top_srcdir)/src/wo -I$(top_srcdir)/src/3d -I$(top_srcdir)/src/gui

libai_a_SOURCES = $(OCAML_INIT) aiinit.hpp
noinst_LIBRARIES = libai.a

GEN	= stringLexer.ml stringParser.mli stringParser.ml requeteLexer.ml requeteParser.mli requeteParser.ml

CML	= stringParser.ml stringLexer.ml requeteLexer.ml requeteParser.ml requeteAffichage.ml requeteSemantique.ml analyseur.ml

CMLI	= requeteSyntax.mli stringParser.mli stringLexer.mli requeteParser.mli requeteLexer.mli requeteAffichage.mli requeteSemantique.mli analyseur.mli

CMI	= $(CMLI:.mli=.cmi)
CMO	= $(CML:.ml=.cmo)
CMX	= $(CML:.ml=.cmx)

####################### Les etapes de compilation #########################
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

clean-generic:
	-@rm -f $(GEN) $(CMO) $(CMI) $(CMX) dummy_.cmx dummy_.cmi Libai.a

endif
