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

TEX_SRCS = img.cpp texture.cpp gif.cpp jpg.cpp png.cpp ppm.cpp pcx.cpp sgi.cpp txf.cpp pgm.cpp bmp.cpp tga.cpp dds.cpp tif.cpp xpm.cpp xbm.cpp psd.cpp avi.cpp svg.cpp glpng.cpp flare.cpp wheel.cpp capture.cpp

TEX_HDRS = img.hpp texture.hpp txf.hpp avi.hpp glpng.hpp flare.hpp wheel.hpp capture.hpp nanosvg.h nanosvgrast.h

SRC = $(top_srcdir)/src
AM_CPPFLAGS = @UBIT_CFLAGS@ @GL_CFLAGS@ -I$(SRC)/base -I$(SRC)/gui -I$(SRC)/world -I$(SRC)/3d -I$(SRC)/net -I$(SRC)/tool

lib2d_a_SOURCES = $(TEX_SRCS) $(TEX_HDRS)
noinst_LIBRARIES = lib2d.a
