AUTOMAKE_OPTIONS = foreign

BASE_SRCS = vreng.cpp pref.cpp env.cpp log.cpp socket.cpp http.cpp url.cpp cache.cpp file.cpp format.cpp trigo.cpp matvec.cpp timer.cpp color.cpp stat.cpp universe.cpp sql.cpp xml.cpp report.cpp thread.cpp nsl.cpp str.cpp base64.cpp endian.cpp

BASE_HDRS = vreng.hpp includes.hpp defines.hpp defaults.hpp macros.hpp pref.hpp env.hpp log.hpp socket.hpp http.hpp url.hpp cache.hpp file.hpp format.hpp trigo.hpp matvec.hpp timer.hpp color.hpp stat.hpp vector3.hpp universe.hpp sql.hpp xml.hpp report.hpp thread.hpp nsl.hpp str.hpp base64.hpp endian.hpp

SRC = $(top_srcdir)/src
CFLAGS = @UBIT_CFLAGS@ @XML2_CFLAGS@ @FREETYPE_CFLAGS@ @SQLITE_CFLAGS@ @MYSQL_CFLAGS@ @OCAML_CFLAGS@
CORE_INCL = -I$(SRC)/ai -I$(SRC)/base -I$(SRC)/ubit -I$(SRC)/gui -I$(SRC)/world -I$(SRC)/net -I$(SRC)/2d -I$(SRC)/model -I$(SRC)/3d -I$(SRC)/vnc -I$(SRC)/tool

AM_CPPFLAGS = -I./src $(CFLAGS) $(CORE_INCL)

EXTRA_DIST = txt2str.c

noinst_PROGRAMS = txt2str
txt2str_SOURCES = txt2str.c

pref.cpp: prefs.h

prefs.h: txt2str $(top_srcdir)/conf/prefs
	./txt2str def_prefs < $(top_srcdir)/conf/prefs > $@

libbase_a_SOURCES = $(BASE_SRCS) $(BASE_HDRS)
noinst_LIBRARIES = libbase.a
