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

AUTOMAKE_OPTIONS = foreign no-dependencies subdir-objects
ACLOCAL_AMFLAGS = -I support

SUBDIRS = src man bin etc

EXTRA_DIST = LICENSE README VERSION.md support/* md/* htdocs/* autogen.sh build vr

ARCHIVE  = /Users/dax/vreng/archive
HTML     = /Users/dax/vreng/html
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
	tar xpf $(TARBALL_GZ)
	@-cp $(TARBALL_GZ) vreng-src.tar.gz
	@-rm $(TARBALLS)

data-tar:
	tar zcf vreng-data.tar.gz htdocs

html-tar:
	tar zcf vreng-html.tar.gz htdocs/html

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

rss:
	support/vreng-rss

.PHONY: dist fingerprint doxygen snapshot history rss data-tar
