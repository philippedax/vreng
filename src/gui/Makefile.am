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

GUI_SRCS = gui.cpp widgets.cpp navig.cpp motion.cpp joystick.cpp panels.cpp palette.cpp theme.cpp message.cpp

GUI_HDRS = gui.hpp widgets.hpp navig.hpp motion.hpp panels.hpp palette.hpp joystick.hpp message.hpp theme.hpp

SRCDIR = $(top_srcdir)/src
AM_CPPFLAGS = @UBIT_CFLAGS@ @GL_CFLAGS@ @X_CFLAGS@ @OCAML_CFLAGS@ @XML2_CFLAGS@ -I$(SRCDIR)/sys -I$(SRCDIR)/vreng -I$(SRCDIR)/wo -I$(SRCDIR)/net -I$(SRCDIR)/2d -I$(SRCDIR)/model -I$(SRCDIR)/3d -I$(SRCDIR)/tool -I$(SRCDIR)/vnc -I$(SRCDIR)/gui/xpm -I$(SRCDIR)/bundle/gl2ps -I$(SRCDIR)/bundle/xmlParser -I$(SRCDIR)/ai -I$(SRCDIR) -I$(SRCDIR)/ubit

widgets.cpp: README.t TODO.t DTD.t COPYRIGHT.t COPYING.t ChangeLog.t CONFIG_H.t

README.t: $(top_srcdir)/README
	../vreng/txt2str README < $(top_srcdir)/README > $@
COPYRIGHT.t: $(top_srcdir)/COPYRIGHT
	../vreng/txt2str COPYRIGHT < $(top_srcdir)/COPYRIGHT > $@
COPYING.t: $(top_srcdir)/COPYING
	../vreng/txt2str COPYING < $(top_srcdir)/COPYING > $@
ChangeLog.t: $(top_srcdir)/ChangeLog
	../vreng/txt2str ChangeLog < $(top_srcdir)/ChangeLog > $@
TODO.t: $(top_srcdir)/TODO
	../vreng/txt2str TODO < $(top_srcdir)/TODO > $@
DTD.t: $(top_srcdir)/md/vre-1.0.dtd
	../vreng/txt2str DTD < $(top_srcdir)/md/vre-1.0.dtd > $@
CONFIG_H.t: $(top_srcdir)/config.h
	../vreng/txt2str CONFIG_H < $(top_srcdir)/config.h > $@

libgui_a_SOURCES  = $(GUI_SRCS) $(GUI_HDRS)
noinst_LIBRARIES = libgui.a

SUBDIRS = xpm
