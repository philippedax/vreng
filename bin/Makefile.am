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
SRCDIR = $(top_srcdir)/src
VRENG_SRCS = $(SRCDIR)/base/vreng.cpp

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
if AM_VRL
vrl: $(SRCDIR)/utils/vrl/vrl
	@-cp -p $(SRCDIR)/utils/vrl/vrl .
else
vrl:
	@echo "" >/dev/null
endif
if AM_VREL
vrel: $(SRCDIR)/utils/vrel/vrel
	@-cp -p $(SRCDIR)/utils/vrel/vrel .
else
vrel:
	@echo "" >/dev/null
endif
if AM_VRED
vred: $(SRCDIR)/utils/vred/vred
	@-cp -p $(SRCDIR)/utils/vred/vred .
else
vred:
	@echo "" >/dev/null
endif
if AM_VACS
vacs: $(SRCDIR)/servers/vacs/vacs
	@-cp -p $(SRCDIR)/servers/vacs/vacs .
else
vacs:
	@echo "" >/dev/null
endif
if AM_VAPS
vaps: $(SRCDIR)/servers/vaps/vaps
	@-cp -p $(SRCDIR)/servers/vaps/vaps .
else
vaps:
	@echo "" >/dev/null
endif
if AM_VJS
vjs: $(SRCDIR)/servers/vjs/vjs
	@-cp -p $(SRCDIR)/servers/vjs/vjs .
else
vjs:
	@echo "" >/dev/null
endif
if AM_VMM
vmm: $(SRCDIR)/servers/vmm/vmm
	@-cp -p $(SRCDIR)/servers/vmm/vmm .
	chmod 755 vmm
else
vmm:
	@echo "" >/dev/null
endif

# core includes
CORE_CFLAGS = -I$(SRCDIR) -I$(SRCDIR)/base -I$(SRCDIR)/ubit -I$(SRCDIR)/gui -I$(SRCDIR)/world -I$(SRCDIR)/net -I$(SRCDIR)/2d -I$(SRCDIR)/3d -I$(SRCDIR)/model -I$(SRCDIR)/tool -I$(SRCDIR)/vnc -I$(SRCDIR)/ai

# ocaml includes
if AM_OCAML
AM_CPPFLAGS = @OCAML_CFLAGS@ @XML2_CFLAGS@ @UBIT_CFLAGS@ $(CORE_CFLAGS)
else
AM_CPPFLAGS = @UBIT_CFLAGS@ $(CORE_CFLAGS)
endif

PNG_LIB= -lpng
Z_LIB= -lz
EXT_LIB = $(PNG_LIB) $(Z_LIB)

# embedded libraries
XML_LIB   = -L$(SRCDIR)/ext/xmlParser $(SRCDIR)/ext/xmlParser/libxmlparser.a
if AM_LIBMPEG
MPEG_LIB  = -L$(SRCDIR)/ext/mpeg_lib $(SRCDIR)/ext/mpeg_lib/libmpeg.a
endif
if AM_FTGL
FTGL_LIB  = -L$(SRCDIR)/ext/ftgl $(SRCDIR)/ext/ftgl/libftgl.a
endif
if AM_GLUI
GLUI_LIB  = -L$(SRCDIR)/ext/glui $(SRCDIR)/ext/glui/libglui.a
endif
if AM_GL2PS
GL2PS_LIB = -L$(SRCDIR)/ext/gl2ps $(SRCDIR)/ext/gl2ps/libgl2ps.a
endif

# core libs
BASE_LIB  = -L$(SRCDIR)/base  $(SRCDIR)/base/libbase.a
GUI_LIB   = -L$(SRCDIR)/gui   $(SRCDIR)/gui/libgui.a
WORLD_LIB = -L$(SRCDIR)/world $(SRCDIR)/world/libworld.a
2D_LIB    = -L$(SRCDIR)/2d    $(SRCDIR)/2d/lib2d.a
3D_LIB    = -L$(SRCDIR)/3d    $(SRCDIR)/3d/lib3d.a
NET_LIB   = -L$(SRCDIR)/net   $(SRCDIR)/net/libnet.a
MODEL_LIB = -L$(SRCDIR)/model $(SRCDIR)/model/libmodel.a
TOOL_LIB  = -L$(SRCDIR)/tool  $(SRCDIR)/tool/libtool.a
VNC_LIB   = -L$(SRCDIR)/vnc   $(SRCDIR)/vnc/libvnc.a
UBIT_LIB  = -L$(SRCDIR)/ubit  $(SRCDIR)/ubit/libubit.a
if AM_OCAML
AI_LIB    = -L$(SRCDIR)/ai    $(SRCDIR)/ai/libai.a @OCAML_LIBS@ -lasmrun $(SRCDIR)/ai/libai.a -lunix -lasmrun -ldl
endif

CORE_LIBS = $(GUI_LIB) $(WORLD_LIB) $(MODEL_LIB) $(2D_LIB) $(3D_LIB) $(NET_LIB) $(TOOL_LIB) $(VNC_LIB) $(BASE_LIB) $(UBIT_LIB) $(AI_LIB)

# embedded libs
EXT_LIBS = $(MPEG_LIB) $(XML_LIB) $(FTGL_LIB) $(GLUI_LIB) $(GL2PS_LIB)

# executable
vreng_LDADD = $(CORE_LIBS) $(EXT_LIBS) $(VRELIBS)
