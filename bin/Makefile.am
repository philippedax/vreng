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
VRENG_SRCS = $(SRCDIR)/vreng/vreng.cpp

# sources
vreng_SOURCES = $(VRENG_SRCS)

# targets
others: vrl vrel vred vacs vaps vjs vmm
all: all-am $(others)

# to install
bin_PROGRAMS = vreng

# to not install
bin_SCRIPTS = vre
noinst_SCRIPTS =

# to copy into bin/
if ENABLE_VRL
vrl: $(SRCDIR)/vrl/vrl
	@-cp -p $(SRCDIR)/vrl/vrl .
else
vrl:
	@echo "" >/dev/null
endif
if ENABLE_VREL
vrel: $(SRCDIR)/vrel/vrel
	@-cp -p $(SRCDIR)/vrel/vrel .
else
vrel:
	@echo "" >/dev/null
endif
if ENABLE_VRED
vred: $(SRCDIR)/vred/vred
	@-cp -p $(SRCDIR)/vred/vred .
else
vred:
	@echo "" >/dev/null
endif
if ENABLE_VACS
vacs: $(SRCDIR)/vacs/vacs
	@-cp -p $(SRCDIR)/vacs/vacs .
else
vacs:
	@echo "" >/dev/null
endif
if ENABLE_VAPS
vaps: $(SRCDIR)/vaps/vaps
	@-cp -p $(SRCDIR)/vaps/vaps .
else
vaps:
	@echo "" >/dev/null
endif
if ENABLE_VJS
vjs: $(SRCDIR)/vjs/vjs
	@-cp -p $(SRCDIR)/vjs/vjs .
else
vjs:
	@echo "" >/dev/null
endif
if ENABLE_VMM
vmm: $(SRCDIR)/vmm/vmm
	@-cp -p $(SRCDIR)/vmm/vmm .
	chmod 755 vmm
else
vmm:
	@echo "" >/dev/null
endif

# core includes
CORE_INCL = -I$(SRCDIR) -I$(SRCDIR)/sys -I$(SRCDIR)/vreng -I$(SRCDIR)/ubit -I$(SRCDIR)/gui -I$(SRCDIR)/wo -I$(SRCDIR)/net -I$(SRCDIR)/2d -I$(SRCDIR)/model -I$(SRCDIR)/3d -I$(SRCDIR)/tool -I$(SRCDIR)/vnc -I$(SRCDIR)/ai

# ocaml includes
if ENABLE_OCAML
AM_CPPFLAGS = @UBIT_CFLAGS@ @OCAML_CFLAGS@ @XML2_CFLAGS@ $(CORE_INCL)
else
AM_CPPFLAGS = @UBIT_CFLAGS@ $(CORE_INCL)
endif

PNG_LIB= -lpng
Z_LIB= -lz
EXT_LIB = $(PNG_LIB) $(Z_LIB)

# bundle libraries
XML_LIB   = -L$(SRCDIR)/bundle/xmlParser $(SRCDIR)/bundle/xmlParser/libxmlparser.a
if ENABLE_LIBMPEG
MPEG_LIB  = -L$(SRCDIR)/bundle/mpeg_lib $(SRCDIR)/bundle/mpeg_lib/libmpeg.a
endif
if ENABLE_FTGL
FTGL_LIB  = -L$(SRCDIR)/bundle/ftgl $(SRCDIR)/bundle/ftgl/libftgl.a
endif
if ENABLE_GLUI
GLUI_LIB  = -L$(SRCDIR)/bundle/glui $(SRCDIR)/bundle/glui/libglui.a
endif
if ENABLE_GL2PS
GL2PS_LIB = -L$(SRCDIR)/bundle/gl2ps $(SRCDIR)/bundle/gl2ps/libgl2ps.a
endif

# core libs
VRENG_LIB = -L$(SRCDIR)/vreng $(SRCDIR)/vreng/libvreng.a
GUI_LIB   = -L$(SRCDIR)/gui   $(SRCDIR)/gui/libgui.a
WO_LIB    = -L$(SRCDIR)/wo    $(SRCDIR)/wo/libwo.a
2D_LIB    = -L$(SRCDIR)/2d    $(SRCDIR)/2d/lib2d.a
3D_LIB    = -L$(SRCDIR)/3d    $(SRCDIR)/3d/lib3d.a
NET_LIB   = -L$(SRCDIR)/net   $(SRCDIR)/net/libnet.a
MODEL_LIB = -L$(SRCDIR)/model $(SRCDIR)/model/libmodel.a
SYS_LIB   = -L$(SRCDIR)/sys   $(SRCDIR)/sys/libsys.a
TOOL_LIB  = -L$(SRCDIR)/tool  $(SRCDIR)/tool/libtool.a
VNC_LIB   = -L$(SRCDIR)/vnc   $(SRCDIR)/vnc/libvnc.a
UBIT_LIB  = -L$(SRCDIR)/ubit  $(SRCDIR)/ubit/libubit.a
if ENABLE_OCAML
AI_LIB    = -L$(SRCDIR)/ai    $(SRCDIR)/ai/libai.a @OCAML_LIBS@ -lasmrun $(SRCDIR)/ai/libai.a -lunix -lasmrun -ldl
endif

CORE_LIBS = $(GUI_LIB) $(WO_LIB) $(MODEL_LIB) $(2D_LIB) $(3D_LIB) $(NET_LIB) $(TOOL_LIB) $(VNC_LIB) $(VRENG_LIB) $(SYS_LIB) $(UBIT_LIB) $(AI_LIB)

# bundle libs
BUNDLE_LIBS = $(MPEG_LIB) $(XML_LIB) $(FTGL_LIB) $(GLUI_LIB) $(GL2PS_LIB)

# executable
vreng_LDADD = $(CORE_LIBS) $(BUNDLE_LIBS) $(VRELIBS)
