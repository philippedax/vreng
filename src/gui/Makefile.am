AUTOMAKE_OPTIONS = foreign

GUI_SRCS = gui.cpp scene.cpp widgets.cpp navig.cpp motion.cpp panels.cpp palette.cpp joystick.cpp message.cpp theme.cpp 

GUI_HDRS = gui.hpp scene.hpp widgets.hpp navig.hpp motion.hpp panels.hpp palette.hpp joystick.hpp message.hpp theme.hpp

SRC = $(top_srcdir)/src
CFLAGS = @UBIT_CFLAGS@ @GL_CFLAGS@ @X_CFLAGS@ @FREETYPE_CFLAGS@ @XML2_CFLAGS@ @OCAML_CFLAGS@
AM_CPPFLAGS = $(CFLAGS) -I$(SRC)/base -I$(SRC)/world -I$(SRC)/net -I$(SRC)/2d -I$(SRC)/model -I$(SRC)/3d -I$(SRC)/tool -I$(SRC)/vnc -I$(SRC)/gui/xpm -I$(SRC)/ext/gl2ps -I$(SRC)/ai -I$(SRC)

widgets.cpp: README.h TODO.h DTD.h COPYRIGHT.h LICENSE.h CHANGELOG.h CONFIG_H.h

TOP = $(top_srcdir)
MD = $(TOP)/md
CONF = $(TOP)/conf

README.h: $(MD)/README.md
	../base/txt2str README < $(MD)/README.md > $@
COPYRIGHT.h: $(MD)/COPYRIGHT.md
	../base/txt2str COPYRIGHT < $(MD)/COPYRIGHT.md > $@
LICENSE.h: $(MD)/LICENSE.md
	../base/txt2str LICENSE < $(MD)/LICENSE.md > $@
CHANGELOG.h: $(MD)/CHANGELOG.md
	../base/txt2str CHANGELOG < $(MD)/CHANGELOG.md > $@
TODO.h: $(MD)/TODO.md
	../base/txt2str TODO < $(MD)/TODO.md > $@
DTD.h: $(CONF)/dtd
	../base/txt2str DTD < $(CONF)/dtd > $@
CONFIG_H.h: $(TOP)/config.h
	../base/txt2str CONFIG_H < $(TOP)/config.h > $@

libgui_a_SOURCES  = $(GUI_SRCS) $(GUI_HDRS)
noinst_LIBRARIES = libgui.a

SUBDIRS = xpm
