## Process this file with automake to create Makefile.in

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
##
## Maintainer: Philippe Dax
##

AUTOMAKE_OPTIONS = foreign no-dependencies
ACLOCAL_AMFLAGS = -I conf

SUBDIRS = src

EXTRA_DIST = conf/* md/* data/* build vr

TARBALL_GZ = $(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.gz
TARBALL_XZ = $(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.xz
TARBALLS = $(TARBALL_GZ) $(TARBALL_XZ)
TARLINK_GZ = $(PACKAGE_NAME).tar.gz
TARLINK_XZ = $(PACKAGE_NAME).tar.xz

#
# maintainer's targets
#
dist dist-all:
	automake -af
	-make dist-gzip
	-make dist-xz
	@-rm compile missing install-sh config.sub config.guess depcomp

archive: dist
	-install -m 444 $(TARBALL_GZ) archive
	-install -m 444 $(TARBALL_XZ) archive
	@-(cd archive; rm -f $(TARLINK_GZ); ln -s $(TARBALL_GZ) $(TARLINK_GZ); rm -f $(TARLINK_XZ); ln -s $(TARBALL_XZ) $(TARLINK_XZ))

clean-tar:
	-tar xpf $(TARBALL_GZ)
	@-cp $(TARBALL_GZ) vreng.tar.gz
	@-rm $(TARBALLS)

data-tar:
	-tar zcf vreng-data.tar.gz data

html-tar:
	-tar zcf vreng-html.tar.gz data/html

doxygen:
	@(umask 022; doxygen conf/doxygen.conf)

snapshot:
	$(MAKE) dist distdir=$(PACKAGE_NAME)`date +"%y%m%d"`

prerequis:
	@(cd conf; ./prerequis.sh)

daemons:
	@(cd conf; ./daemons.sh)

cache:
	@(cd conf; ./cache.sh)

clone:
	@(conf/git.sh clone)

history:
	@(cd archive; ls -ltr)

.PHONY: dist clean-tar data-tar html-tar doxygen snapshot prerequis daemons cache clone history
