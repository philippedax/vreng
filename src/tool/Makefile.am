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

APP_SRCS = tool.cpp audio.cpp xterm.cpp video.cpp wb.cpp browser.cpp ps.cpp pdf.cpp mp3.cpp wav.cpp au.cpp midi.cpp mpeg.cpp wget.cpp modeler.cpp office.cpp

APP_HDRS = tool.hpp au.hpp audio.hpp video.hpp wb.hpp browser.hpp midi.hpp modeler.hpp mp3.hpp mpeg.hpp office.hpp pdf.hpp ps.hpp wav.hpp wget.hpp xterm.hpp

SRCDIR = $(top_srcdir)/src
AM_CPPFLAGS = -I$(SRCDIR)/vreng -I$(SRCDIR)/wo -I$(SRCDIR)/net

libtool_a_SOURCES = $(APP_SRCS) $(APP_HDRS)
noinst_LIBRARIES = libtool.a
