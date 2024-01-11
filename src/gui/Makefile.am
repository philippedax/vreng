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

AUTOMAKE_OPTIONS = foreign

GUI_SRCS = gui.cpp scene.cpp widgets.cpp navig.cpp motion.cpp joystick.cpp panels.cpp palette.cpp theme.cpp message.cpp

GUI_HDRS = gui.hpp scene.hpp widgets.hpp navig.hpp motion.hpp panels.hpp palette.hpp joystick.hpp message.hpp theme.hpp

SRC = $(top_srcdir)/src
CFLAGS = @UBIT_CFLAGS@ @GL_CFLAGS@ @X_CFLAGS@ @FREETYPE_CFLAGS@ @XML2_CFLAGS@ @OCAML_CFLAGS@
AM_CPPFLAGS = $(CFLAGS) -I$(SRC)/base -I$(SRC)/world -I$(SRC)/net -I$(SRC)/2d -I$(SRC)/model -I$(SRC)/3d -I$(SRC)/tool -I$(SRC)/vnc -I$(SRC)/gui/xpm -I$(SRC)/ext/gl2ps -I$(SRC)/ai -I$(SRC)

widgets.cpp: README.h TODO.h DTD.h COPYRIGHT.h LICENSE.h ChangeLog.h CONFIG_H.h
TOP = $(top_srcdir)
MD = $(TOP)/md

README.h: $(MD)/README.md
	../base/txt2str README < $(MD)/README.md > $@
COPYRIGHT.h: $(MD)/COPYRIGHT.md
	../base/txt2str COPYRIGHT < $(MD)/COPYRIGHT.md > $@
LICENSE.h: $(MD)/LICENSE.md
	../base/txt2str LICENSE < $(MD)/LICENSE.md > $@
ChangeLog.h: $(MD)/ChangeLog.md
	../base/txt2str ChangeLog < $(MD)/ChangeLog.md > $@
TODO.h: $(MD)/TODO.md
	../base/txt2str TODO < $(MD)/TODO.md > $@
DTD.h: $(MD)/vre-1.0.dtd
	../base/txt2str DTD < $(MD)/vre-1.0.dtd > $@
CONFIG_H.h: $(TOP)/config.h
	../base/txt2str CONFIG_H < $(TOP)/config.h > $@

libgui_a_SOURCES  = $(GUI_SRCS) $(GUI_HDRS)
noinst_LIBRARIES = libgui.a

SUBDIRS = xpm
