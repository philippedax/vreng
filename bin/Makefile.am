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
all: all-am vrel vred

# to install
bin_PROGRAMS = vreng

# to not install
bin_SCRIPTS = vre
noinst_SCRIPTS =

# to copy into bin/
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

# core includes
CORE_INCL = -I$(SRCDIR) -I$(SRCDIR)/sys -I$(SRCDIR)/vreng -I$(SRCDIR)/ubit -I$(SRCDIR)/gui -I$(SRCDIR)/wo -I$(SRCDIR)/net -I$(SRCDIR)/texture -I$(SRCDIR)/model -I$(SRCDIR)/vgl -I$(SRCDIR)/app -I$(SRCDIR)/vnc -I$(SRCDIR)/ai

# ocaml includes
if OCAML
AM_CPPFLAGS = @UBIT_CFLAGS@ @OCAML_CFLAGS@ @XML2_CFLAGS@ $(CORE_INCL)
else
AM_CPPFLAGS = @UBIT_CFLAGS@ $(CORE_INCL)
endif

# bundle includes
if BUNDLE_LIBZ
ZLIB=-L$(SRCDIR)/bundle/zlib $(SRCDIR)/bundle/zlib/libz.a
else
ZLIB= -lz
endif
if BUNDLE_LIBPNG
PNGLIB=-L$(SRCDIR)/bundle/png $(SRCDIR)/bundle/png/libpng.a
else
PNGLIB= -lpng
endif
if BUNDLE_LIBMPEG
MPEGLIB=-L$(SRCDIR)/bundle/mpeg_lib $(SRCDIR)/bundle/mpeg_lib/libmpeg.a
else
MPEGLIB=
endif
GLPNGLIB=-L$(SRCDIR)/bundle/glpng $(SRCDIR)/bundle/glpng/libglpng.a
XMLLIB=-L$(SRCDIR)/bundle/xmlParser $(SRCDIR)/bundle/xmlParser/libxmlparser.a
if BUNDLE_GL2PS
GL2PSLIB=-L$(SRCDIR)/bundle/gl2ps $(SRCDIR)/bundle/gl2ps/libgl2ps.a
else
GL2PSLIB=
endif
if OCAML
AILIB=-L$(SRCDIR)/ai $(SRCDIR)/ai/libai.a @OCAML_LIBS@ -lasmrun $(SRCDIR)/ai/libai.a -lunix -lasmrun -ldl
else
AILIB=
endif
if BUNDLE_GLUI
GLUILIB=-L$(SRCDIR)/bundle/glui $(SRCDIR)/bundle/glui/libglui.a
else
GLUILIB= 
endif
if UBIT
UBITLIB=-L$(SRCDIR)/ubit $(SRCDIR)/ubit/libubit.a
else
UBITLIB= -lubit
endif
if BUNDLE_FTGL
FTGLLIB=-L$(SRCDIR)/bundle/ftgl $(SRCDIR)/bundle/ftgl/libftgl.a
else
FTGLLIB= 
endif

# core libs
VRENGLIB  = -L$(SRCDIR)/vreng $(SRCDIR)/vreng/libvreng.a
SYSLIB    = -L$(SRCDIR)/sys $(SRCDIR)/sys/libsys.a
GUILIB    = -L$(SRCDIR)/gui $(SRCDIR)/gui/libgui.a
WOLIB     = -L$(SRCDIR)/wo  $(SRCDIR)/wo/libwo.a
NETLIB    = -L$(SRCDIR)/net $(SRCDIR)/net/libnet.a
TEXLIB    = -L$(SRCDIR)/texture $(SRCDIR)/texture/libtexture.a
MODELLIB  = -L$(SRCDIR)/model $(SRCDIR)/model/libmodel.a
VGLLIB    = -L$(SRCDIR)/vgl $(SRCDIR)/vgl/libvgl.a
APPLIB    = -L$(SRCDIR)/app $(SRCDIR)/app/libapp.a
VNCLIB    = -L$(SRCDIR)/vnc $(SRCDIR)/vnc/libvnc.a
CORE_LIBS = $(GUILIB) $(WOLIB) $(NETLIB) $(TEXLIB) $(MODELLIB) $(VGLLIB) $(APPLIB) $(VNCLIB) $(VRENGLIB) $(SYSLIB)

# bundle libs
if BUNDLE_LIBPNG
BUNDLE_LIBS = $(ZLIB) $(PNGLIB) $(GLPNGLIB) $(MPEGLIB) $(AILIB) $(GL2PSLIB) $(GLUILIB) $(UBITLIB) $(FTGLLIB) $(XMLLIB) $(VRELIBS) $(PNGLIB) $(XLIBS)
else
BUNDLE_LIBS = $(ZLIB) $(PNGLIB) $(GLPNGLIB) $(MPEGLIB) $(AILIB) $(GL2PSLIB) $(GLUILIB) $(UBITLIB) $(FTGLLIB) $(XMLLIB) $(VRELIBS) $(XLIBS)
endif

# executable
vreng_LDADD = $(CORE_LIBS) $(BUNDLE_LIBS)
