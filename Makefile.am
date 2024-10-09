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
##
## Maintainer: Philippe Dax
##

AUTOMAKE_OPTIONS = foreign no-dependencies
ACLOCAL_AMFLAGS = -I conf

SUBDIRS = src man

EXTRA_DIST = conf/* md/* data/* build vr

# paths for sources
SRC = $(top_srcdir)/src
VRENG_SRC = $(SRC)/base/vreng.cpp

# sources
vreng_SOURCES = $(VRENG_SRC)

#
# targets
#
MAIN_TARGET = all-am
OTHER_TARGETS = vrl vrel vred vacs vaps vjs vmm
all: $(MAIN_TARGET) $(OTHER_TARGETS)

# to install
bin_PROGRAMS = vreng

# to not install
#bin_SCRIPTS = vre
#noinst_SCRIPTS =

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

#
# core libs
#
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
AI_LIB    = -L$(SRC)/ai    $(SRC)/ai/libai.a @OCAML_LIB@ -lasmrun $(SRC)/ai/libai.a -lunix -lasmrun
endif

CORE_LIBS = $(BASE_LIB) $(GUI_LIB) $(WORLD_LIB) $(MODEL_LIB) $(2D_LIB) $(3D_LIB) $(NET_LIB) $(TOOL_LIB) $(VNC_LIB) $(UBIT_LIB) $(AI_LIB)

#
# embedded libraries
#
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

EXT_LIBS = $(MPEG_LIB) $(XML_LIB) $(FTGL_LIB) $(GLUI_LIB) $(GL2PS_LIB) $(TINYGL_LIB)

#
# executable
#
vreng_LDADD = $(CORE_LIBS) $(EXT_LIBS) -lm


ARCHIVE  = archive
HTML     = html
#HTTP_SVN = http://www.vreng.enst.fr/svn/vreng

TARBALL_GZ = $(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.gz
TARBALL_XZ = $(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.xz
TARBALLS = $(TARBALL_GZ) $(TARBALL_XZ)

TARLINK_GZ = $(PACKAGE_NAME).tar.gz
TARLINK_XZ = $(PACKAGE_NAME).tar.xz

#FINGERPRINT_GZ = $(PACKAGE_NAME)-$(PACKAGE_VERSION)-gz.md5sum
#FINGERPRINT_XZ = $(PACKAGE_NAME)-$(PACKAGE_VERSION)-xz.md5sum

## maintainer's targets

dist dist-all:
	-make dist-gzip
	-make dist-xz
	@-cp -p ChangeLog VERSION.md $(HTML)

#fingerprint:
#	-md5sum < $(TARBALL_GZ) > $(FINGERPRINT_GZ)
#	-md5sum < $(TARBALL_XZ) > $(FINGERPRINT_XZ)

clean-tar:
	-tar xpf $(TARBALL_GZ)
	@-cp $(TARBALL_GZ) vreng.tar.gz
	@-rm $(TARBALLS)

data-tar:
	tar zcf vreng-data.tar.gz data

html-tar:
	tar zcf vreng-html.tar.gz data/html

archive: dist
	-install -m 444 $(TARBALL_GZ) $(ARCHIVE)
	-install -m 444 $(TARBALL_XZ) $(ARCHIVE)
	@-(cd $(ARCHIVE); rm -f $(TARLINK_GZ); ln -s $(TARBALL_GZ) $(TARLINK_GZ); rm -f $(TARLINK_XZ); ln -s $(TARBALL_XZ) $(TARLINK_XZ))
#	-install -m 444 $(FINGERPRINT_GZ) $(ARCHIVE)
#	-install -m 444 $(FINGERPRINT_XZ) $(ARCHIVE)
#	@-rm $(FINGERPRINT_GZ)
#	@-rm $(FINGERPRINT_XZ)

doxygen:
	@-rm -f $(HTML)/doxygen/html/*
	@(umask 022; doxygen md/vreng.dox)

snapshot:
	$(MAKE) dist distdir=$(PACKAGE_NAME)`date +"%y%m%d"`

history:
	@(cd $(ARCHIVE); ls -ltr)

.PHONY: dist fingerprint doxygen snapshot history data-tar
