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

EXTRA_DIST = gestures.hpp man.data

MODEL_SRCS = md2.cpp lwo.cpp off.cpp 3ds.cpp ase.cpp man.cpp obj.cpp dxf.cpp x3d.cpp vec3.cpp body.cpp face.cpp bap.cpp fap.cpp bvh.cpp bone.cpp car.cpp teapot.cpp

MODEL_HDRS = md2.hpp lwo.hpp off.hpp 3ds.hpp ase.hpp man.hpp obj.hpp dxf.hpp x3d.hpp vec3.hpp body.hpp face.hpp bap.hpp fap.hpp bvh.hpp bone.hpp car.hpp teapot.hpp

SRC = $(top_srcdir)/src
CFLAGS = @UBIT_CFLAGS@ @GL_CFLAGS@
AM_CPPFLAGS = $(CFLAGS) -I$(SRC)/base -I$(SRC)/gui -I$(SRC)/world -I$(SRC)/net -I$(SRC)/2d -I$(SRC)/3d -I$(SRC)/ext/xmlParser

libmodel_a_SOURCES = $(MODEL_SRCS) $(MODEL_HDRS)
noinst_LIBRARIES = libmodel.a
