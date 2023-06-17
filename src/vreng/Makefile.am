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

VRENG_SRCS = vreng.cpp scene.cpp pref.cpp env.cpp log.cpp socket.cpp http.cpp url.cpp reader.cpp cache.cpp file.cpp format.cpp trigo.cpp matvec.cpp timer.cpp color.cpp stat.cpp sound.cpp sounds.cpp universe.cpp vrsql.cpp xml.cpp openal.cpp report.cpp thread.cpp nsl.cpp str.cpp

VRENG_HDRS = vreng.hpp scene.hpp defaults.hpp macros.hpp pref.hpp env.hpp log.hpp socket.hpp http.hpp url.hpp reader.hpp cache.hpp file.hpp format.hpp trigo.hpp matvec.hpp timer.hpp color.hpp stat.hpp sound.hpp sounds.hpp vector3.hpp universe.hpp vrsql.hpp xml.hpp openal.hpp report.hpp thread.hpp nsl.hpp str.hpp

SRCDIR = $(top_srcdir)/src
CORE_INCL = -I$(SRCDIR)/sys -I$(SRCDIR)/vreng -I$(SRCDIR)/ubit -I$(SRCDIR)/gui -I$(SRCDIR)/wo -I$(SRCDIR)/net -I$(SRCDIR)/2d -I$(SRCDIR)/model -I$(SRCDIR)/3d -I$(SRCDIR)/vnc -I$(SRCDIR)/tool -I$(SRCDIR)/ai
if ENABLE_OCAML
AM_CPPFLAGS = @UBIT_CFLAGS@ @XML2_CFLAGS@ $(CORE_INCL) @OCAML_CFLAGS@ -I$(SRCDIR)/ai
else
AM_CPPFLAGS = @UBIT_CFLAGS@ @XML2_CFLAGS@ $(CORE_INCL)
endif

EXTRA_DIST = txt2str.c

noinst_PROGRAMS = txt2str
txt2str_SOURCES = txt2str.c

pref.cpp: prefs.t

prefs.t: txt2str $(top_srcdir)/md/prefs
	./txt2str def_prefs < $(top_srcdir)/md/prefs > $@

libvreng_a_SOURCES = $(VRENG_SRCS) $(VRENG_HDRS)
noinst_LIBRARIES = libvreng.a
