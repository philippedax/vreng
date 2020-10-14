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

WO_SRCS = oclass.cpp wobject.cpp vr2gl.cpp initobj.cpp parse.cpp olist.cpp move.cpp col.cpp names.cpp user.cpp gate.cpp wall.cpp door.cpp mirage.cpp cauldron.cpp ball.cpp step.cpp host.cpp board.cpp web.cpp button.cpp animator.cpp bullet.cpp lift.cpp aoi.cpp doc.cpp entry.cpp stars.cpp bgcolor.cpp music.cpp mirror.cpp clock.cpp clip.cpp ftp.cpp water.cpp dart.cpp mech.cpp light.cpp text.cpp movie.cpp thing.cpp guy.cpp walls.cpp android.cpp vnc.cpp vrelet.cpp vjc.cpp transform.cpp model.cpp mysql.cpp carrier.cpp cart.cpp particle.cpp book.cpp sheet.cpp icon.cpp flag.cpp guide.cpp area.cpp projectile.cpp ground.cpp plane.cpp animx3d.cpp wind.cpp widget.cpp thunder.cpp cloud.cpp stair.cpp escalator.cpp travelator.cpp spiral.cpp waterfall.cpp fountain.cpp firework.cpp rain.cpp snow.cpp bubble.cpp terrain.cpp river.cpp sun.cpp cloth.cpp halo.cpp hat.cpp dress.cpp wings.cpp fire.cpp smoke.cpp hairs.cpp slider.cpp bumper.cpp accessory.cpp head.cpp fractal.hpp

WO_HDRS = oclass.hpp wobject.hpp objects.hpp initobj.hpp parse.hpp olist.hpp vre.hpp move.hpp user.hpp wall.hpp ball.hpp board.hpp cauldron.hpp door.hpp mirage.hpp gate.hpp host.hpp step.hpp web.hpp button.hpp walls.hpp animator.hpp bullet.hpp lift.hpp aoi.hpp doc.hpp entry.hpp stars.hpp bgcolor.hpp music.hpp mirror.hpp clock.hpp clip.hpp ftp.hpp water.hpp dart.hpp mech.hpp light.hpp text.hpp movie.hpp guy.hpp thing.hpp android.hpp vnc.hpp vrelet.hpp vjc.hpp transform.hpp model.hpp mysql.hpp carrier.hpp cart.hpp particle.hpp book.hpp sheet.hpp icon.hpp flag.hpp guide.hpp area.hpp projectile.hpp ground.hpp plane.hpp animx3d.hpp wind.hpp widget.hpp thunder.hpp cloud.hpp stair.hpp escalator.hpp travelator.hpp spiral.hpp waterfall.hpp fountain.hpp firework.hpp rain.hpp snow.hpp bubble.hpp terrain.hpp river.hpp sun.hpp cloth.hpp halo.hpp hat.hpp dress.hpp wings.hpp fire.hpp smoke.hpp hairs.hpp slider.hpp bumper.hpp accessory.hpp head.hpp fractal.cpp

EXTRA_DIST = template.hpp template.cpp

SRCDIR = $(top_srcdir)/src
AM_CPPFLAGS = @UBIT_CFLAGS@ @XML2_CFLAGS@ @MYSQL_CFLAGS@ -I$(SRCDIR)/sys -I$(SRCDIR)/vreng -I$(SRCDIR)/gui -I$(SRCDIR)/net -I$(SRCDIR)/texture -I$(SRCDIR)/model -I$(SRCDIR)/vgl -I$(SRCDIR)/app -I$(SRCDIR)/vnc -I$(SRCDIR)/bundle/xmlParser -I$(SRCDIR)/bundle/mpeg_lib

libwo_a_SOURCES = $(WO_SRCS) $(WO_HDRS)
noinst_LIBRARIES = libwo.a
