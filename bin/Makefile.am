## Process this file with automake to create Makefile.in

## Makefile for Automake.

## Copyright (C) 1995, 1996, 1997, 1998, 1999, 2001, 2002, 2003, 2004
## Free Software Foundation, Inc.

## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.

## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.

## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

AUTOMAKE_OPTIONS = foreign no-dependencies

# paths for sources
SRC = $(top_srcdir)/src
VRENG_SRCS = $(SRC)/base/vreng.cpp

# sources
vreng_SOURCES = $(VRENG_SRCS)

# targets
MAIN_TARGET = all-am
OTHER_TARGETS = vrl vrel vred vacs vaps vjs vmm
all: $(MAIN_TARGET) $(OTHER_TARGETS)

# to install
bin_PROGRAMS = vreng

# to not install
bin_SCRIPTS = vre
noinst_SCRIPTS =

# to copy into bin/
if ENABLE_VRL
vrl: $(SRC)/utils/vrl/vrl
	@-cp -p $(SRC)/utils/vrl/vrl .
else
vrl:
	@echo "" >/dev/null
endif
if ENABLE_VREL
vrel: $(SRC)/utils/vrel/vrel
	@-cp -p $(SRC)/utils/vrel/vrel .
else
vrel:
	@echo "" >/dev/null
endif
if ENABLE_VRED
vred: $(SRC)/utils/vred/vred
	@-cp -p $(SRC)/utils/vred/vred .
else
vred:
	@echo "" >/dev/null
endif
if ENABLE_VACS
vacs: $(SRC)/servers/vacs/vacs
	@-cp -p $(SRC)/servers/vacs/vacs .
else
vacs:
	@echo "" >/dev/null
endif
if ENABLE_VAPS
vaps: $(SRC)/servers/vaps/vaps
	@-cp -p $(SRC)/servers/vaps/vaps .
else
vaps:
	@echo "" >/dev/null
endif
if ENABLE_VJS
vjs: $(SRC)/servers/vjs/vjs
	@-cp -p $(SRC)/servers/vjs/vjs .
else
vjs:
	@echo "" >/dev/null
endif
if ENABLE_VMM
vmm: $(SRC)/servers/vmm/vmm
	@-cp -p $(SRC)/servers/vmm/vmm .
	chmod 755 vmm
else
vmm:
	@echo "" >/dev/null
endif

# core includes
CORE_CFLAGS = -I$(SRC) -I$(SRC)/base -I$(SRC)/ubit -I$(SRC)/gui -I$(SRC)/world -I$(SRC)/net -I$(SRC)/2d -I$(SRC)/3d -I$(SRC)/model -I$(SRC)/tool -I$(SRC)/vnc -I$(SRC)/ai

# ocaml includes
if ENABLE_OCAML
#AM_CPPFLAGS = $(CORE_CFLAGS) @XML2_CFLAGS@ @UBIT_CFLAGS@ @OCAML_CFLAGS@
else
#AM_CPPFLAGS = $(CORE_CFLAGS) @XML2_CFLAGS@ @UBIT_CFLAGS@
endif

PNG_LIB= -lpng
Z_LIB= -lz
EXT_LIB = $(PNG_LIB) $(Z_LIB)

# core libs
BASE_LIB  = -L$(SRC)/base  $(SRC)/base/libbase.a
GUI_LIB   = -L$(SRC)/gui   $(SRC)/gui/libgui.a
WORLD_LIB = -L$(SRC)/world $(SRC)/world/libworld.a
2D_LIB    = -L$(SRC)/2d    $(SRC)/2d/lib2d.a
3D_LIB    = -L$(SRC)/3d    $(SRC)/3d/lib3d.a
NET_LIB   = -L$(SRC)/net   $(SRC)/net/libnet.a
MODEL_LIB = -L$(SRC)/model $(SRC)/model/libmodel.a
TOOL_LIB  = -L$(SRC)/tool  $(SRC)/tool/libtool.a
VNC_LIB   = -L$(SRC)/vnc   $(SRC)/vnc/libvnc.a
UBIT_LIB  = -L$(SRC)/ubit  $(SRC)/ubit/libubit.a
if ENABLE_OCAML
AI_LIB    = -L$(SRC)/ai    $(SRC)/ai/libai.a @OCAML_LIBS@ -lasmrun $(SRC)/ai/libai.a -lunix -lasmrun -ldl
endif

CORE_LIBS = $(GUI_LIB) $(WORLD_LIB) $(MODEL_LIB) $(2D_LIB) $(3D_LIB) $(NET_LIB) $(TOOL_LIB) $(VNC_LIB) $(BASE_LIB) $(UBIT_LIB) $(AI_LIB)

# embedded libraries
XML_LIB   = -L$(SRC)/ext/xmlParser $(SRC)/ext/xmlParser/libxmlparser.a
if ENABLE_LIBMPEG
MPEG_LIB  = -L$(SRC)/ext/mpeg_lib $(SRC)/ext/mpeg_lib/libmpeg.a
endif
if ENABLE_FTGL
FTGL_LIB  = -L$(SRC)/ext/ftgl $(SRC)/ext/ftgl/libftgl.a
endif
if ENABLE_GLUI
GLUI_LIB  = -L$(SRC)/ext/glui $(SRC)/ext/glui/libglui.a
endif
if ENABLE_GL2PS
GL2PS_LIB = -L$(SRC)/ext/gl2ps $(SRC)/ext/gl2ps/libgl2ps.a
endif
if ENABLE_TINYGL
TINYGL_LIB = -L$(SRC)/ext/TinyGL $(SRC)/ext/TinyGL/libTinyGL.a
endif

# embedded libs
EXT_LIBS = $(MPEG_LIB) $(XML_LIB) $(FTGL_LIB) $(GLUI_LIB) $(GL2PS_LIB) $(TINYGL_LIB)

# executable
VRELIBS += -lpthread
vreng_LDADD = $(CORE_LIBS) $(EXT_LIBS) $(VRELIBS)
