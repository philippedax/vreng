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

EXTRA_DIST = vreng.cpp txt2str.c

VRENG_SRCS = vreng.cpp pref.cpp env.cpp log.cpp socket.cpp http.cpp url.cpp reader.cpp cache.cpp file.cpp format.cpp trigo.cpp matvec.cpp timer.cpp color.cpp prof.cpp stat.cpp sound.cpp sounds.cpp

VRENG_HDRS = vreng.hpp defaults.hpp macros.hpp pref.hpp env.hpp log.hpp socket.hpp http.hpp url.hpp reader.hpp cache.hpp file.hpp format.hpp trigo.hpp matvec.hpp timer.hpp prof.hpp color.hpp stat.hpp sound.hpp sounds.hpp vector3.hpp

SRCDIR = $(top_srcdir)/src
CORE_INCL = -I$(SRCDIR)/sys -I$(SRCDIR)/vreng -I$(SRCDIR)/ubit -I$(SRCDIR)/gui -I$(SRCDIR)/wo -I$(SRCDIR)/net -I$(SRCDIR)/texture -I$(SRCDIR)/model -I$(SRCDIR)/vgl -I$(SRCDIR)/app -I$(SRCDIR)/vnc -I$(SRCDIR)/ai
if OCAML
AM_CPPFLAGS = @UBIT_CFLAGS@ @OCAML_CFLAGS@ @XML2_CFLAGS@ $(CORE_INCL) -I$(SRCDIR)/ai
else
AM_CPPFLAGS = @UBIT_CFLAGS@ $(CORE_INCL)
endif

noinst_PROGRAMS = txt2str
txt2str_SOURCES = txt2str.c

pref.cpp: prefs.t

prefs.t: txt2str $(top_srcdir)/conf/prefs
	./txt2str def_prefs < $(top_srcdir)/conf/prefs > $@

libvreng_a_SOURCES = $(VRENG_SRCS) $(VRENG_HDRS)
noinst_LIBRARIES = libvreng.a
