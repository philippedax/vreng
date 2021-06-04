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
all: all-am vrl vrel vred vacs vaps vjs vmm

# to install
bin_PROGRAMS = vreng

# to not install
bin_SCRIPTS = vre
noinst_SCRIPTS =

# to copy into bin/
if VRL
vrl: ../src/util/vrl/src/vrl
	-cp -p ../src/util/vrl/src/vrl .
else
vrl:
	@echo "" >/dev/null
endif
if VREL
vrel: ../src/util/vrel/vrel
	-cp -p ../src/util/vrel/vrel .
else
vrel:
	@echo "" >/dev/null
endif
if VRED
vred: ../src/util/vred/vred
	-cp -p ../src/util/vred/vred .
else
vred:
	@echo "" >/dev/null
endif
if VMM
vmm: ../src/util/vmm/vmm
	-cp -p ../src/util/vmm/vmm .
else
vmm:
	@echo "" >/dev/null
endif
if ENABLE_VACS
vacs: ../src/servers/vacs/vacs
	-cp -p ../src/servers/vacs/vacs .
else
vacs:
	@echo "" >/dev/null
endif
if ENABLE_VAPS
vaps: ../src/servers/vaps/vaps
	-cp -p ../src/servers/vaps/vaps .
else
vaps:
	@echo "" >/dev/null
endif
if ENABLE_VJS
vjs: ../src/servers/vjs/vjs
	-cp -p ../src/servers/vjs/vjs .
else
vjs:
	@echo "" >/dev/null
endif

# core includes
CORE_INCL = -I$(SRCDIR) -I$(SRCDIR)/sys -I$(SRCDIR)/vreng -I$(SRCDIR)/ubit -I$(SRCDIR)/gui -I$(SRCDIR)/wo -I$(SRCDIR)/net -I$(SRCDIR)/2d -I$(SRCDIR)/model -I$(SRCDIR)/3d -I$(SRCDIR)/tool -I$(SRCDIR)/vnc -I$(SRCDIR)/ai

# ocaml includes
if OCAML
AM_CPPFLAGS = @UBIT_CFLAGS@ @OCAML_CFLAGS@ @XML2_CFLAGS@ $(CORE_INCL)
else
AM_CPPFLAGS = @UBIT_CFLAGS@ $(CORE_INCL)
endif

# bundle libraries
if BUNDLE_LIBZ
Z_LIB=-L$(SRCDIR)/bundle/zlib $(SRCDIR)/bundle/zlib/libz.a
else
Z_LIB= -lz
endif
if BUNDLE_LIBPNG
PNG_LIB=-L$(SRCDIR)/bundle/mypng $(SRCDIR)/bundle/mypng/libpng.a
else
PNG_LIB= -lpng
endif
if BUNDLE_LIBMPEG
MPEG_LIB=-L$(SRCDIR)/bundle/mpeg_lib $(SRCDIR)/bundle/mpeg_lib/libmpeg.a
else
MPEG_LIB=
endif
XML_LIB=-L$(SRCDIR)/bundle/xmlParser $(SRCDIR)/bundle/xmlParser/libxmlparser.a
if BUNDLE_GL2PS
GL2PS_LIB=-L$(SRCDIR)/bundle/gl2ps $(SRCDIR)/bundle/gl2ps/libgl2ps.a
else
GL2PS_LIB=
endif
if BUNDLE_GLUI
GLUI_LIB=-L$(SRCDIR)/bundle/glui $(SRCDIR)/bundle/glui/libglui.a
else
GLUI_LIB= 
endif
if BUNDLE_FTGL
FTGL_LIB=-L$(SRCDIR)/bundle/ftgl $(SRCDIR)/bundle/ftgl/libftgl.a
else
FTGL_LIB= 
endif

# core libs
VRENG_LIB  = -L$(SRCDIR)/vreng $(SRCDIR)/vreng/libvreng.a
SYS_LIB    = -L$(SRCDIR)/sys $(SRCDIR)/sys/libsys.a
GUI_LIB    = -L$(SRCDIR)/gui $(SRCDIR)/gui/libgui.a
WO_LIB     = -L$(SRCDIR)/wo  $(SRCDIR)/wo/libwo.a
MODEL_LIB  = -L$(SRCDIR)/model $(SRCDIR)/model/libmodel.a
2D_LIB     = -L$(SRCDIR)/2d $(SRCDIR)/2d/lib2d.a
3D_LIB     = -L$(SRCDIR)/3d $(SRCDIR)/3d/lib3d.a
NET_LIB    = -L$(SRCDIR)/net $(SRCDIR)/net/libnet.a
TOOL_LIB   = -L$(SRCDIR)/tool $(SRCDIR)/tool/libtool.a
VNC_LIB    = -L$(SRCDIR)/vnc $(SRCDIR)/vnc/libvnc.a
if UBIT
UBIT_LIB=-L$(SRCDIR)/ubit $(SRCDIR)/ubit/libubit.a
else
UBIT_LIB= -lubit
endif
if OCAML
AI_LIB=-L$(SRCDIR)/ai $(SRCDIR)/ai/libai.a @OCAML_LIBS@ -lasmrun $(SRCDIR)/ai/libai.a -lunix -lasmrun -ldl
else
AI_LIB=
endif
CORE_LIBS = $(GUI_LIB) $(WO_LIB) $(MODEL_LIB) $(2D_LIB) $(3D_LIB) $(NET_LIB) $(TOOL_LIB) $(VNC_LIB) $(VRENG_LIB) $(SYS_LIB) $(UBIT_LIB) $(AI_LIB)

# bundle libs
BUNDLE_LIBS = $(Z_LIB) $(GLPNG_LIB) $(MPEG_LIB) $(GL2PS_LIB) $(GLUI_LIB) $(FTGL_LIB) $(XML_LIB) $(PNG_LIB)

# executable
vreng_LDADD = $(CORE_LIBS) $(BUNDLE_LIBS) $(VRELIBS) $(XLIBS)
