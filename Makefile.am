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

EXTRA_DIST = COPYRIGHT VERSION support/* md/* autogen.sh vr

## maintainer's targets

ARCHIVE  = /Users/dax/vreng/archive
HTML     = /Users/dax/vreng/html
HTTP_SVN = http://www.vreng.enst.fr/svn/vreng

TARBALL_GZ = $(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.gz
TARBALL_XZ = $(PACKAGE_NAME)-$(PACKAGE_VERSION).tar.xz
TARBALL_ZIP = $(PACKAGE_NAME)-$(PACKAGE_VERSION).zip
TARBALLS = $(TARBALL_GZ) $(TARBALL_XZ) $(TARBALL_ZIP)

TARLINK_GZ = $(PACKAGE_NAME).tar.gz
TARLINK_XZ = $(PACKAGE_NAME).tar.xz
TARLINK_ZIP = $(PACKAGE_NAME).zip

FINGERPRINT_GZ = $(PACKAGE_NAME)-$(PACKAGE_VERSION)-gz.md5sum
FINGERPRINT_XZ = $(PACKAGE_NAME)-$(PACKAGE_VERSION)-xz.md5sum
FINGERPRINT_ZIP = $(PACKAGE_NAME)-$(PACKAGE_VERSION)-zip.md5sum

dist dist-all:
	-make dist-gzip
	-make dist-xz
#	-make dist-bzip2
#	-make dist-zip
	-cp -p ChangeLog VERSION $(HTML)

fingerprint:
	-md5sum < $(TARBALL_GZ) > $(FINGERPRINT_GZ)
	-md5sum < $(TARBALL_XZ) > $(FINGERPRINT_XZ)
#	-md5sum < $(TARBALL_ZIP) > $(FINGERPRINT_ZIP)

clean-tar:
	tar xpf $(TARBALL_GZ)
	cp $(TARBALL_GZ) vreng.tar.gz
	rm $(TARBALLS)

data-tar:
	tar zcf vreng-data.tar.gz htdocs

html-tar:
	tar zcf vreng-html.tar.gz htdocs/html

svntags:
	su -c "svn copy $(HTTP_SVN)/trunk $(HTTP_SVN)/tags/$(PACKAGE_NAME)-$(PACKAGE_VERSION) -m \"Distribution: $(PACKAGE_NAME)-$(PACKAGE_VERSION)\"" dax

archive: dist fingerprint
	-install -m 444 $(FINGERPRINT_GZ) $(ARCHIVE)
	-install -m 444 $(TARBALL_GZ) $(ARCHIVE)
	-install -m 444 $(FINGERPRINT_XZ) $(ARCHIVE)
	-install -m 444 $(TARBALL_XZ) $(ARCHIVE)
#	-install -m 444 $(FINGERPRINT_ZIP) $(ARCHIVE)
#	-install -m 444 $(TARBALL_ZIP) $(ARCHIVE)
	-(cd $(ARCHIVE); rm -f $(TARLINK_GZ); ln -s $(TARBALL_GZ) $(TARLINK_GZ); rm -f $(TARLINK_XZ); ln -s $(TARBALL_XZ) $(TARLINK_XZ); rm -f $(TARLINK_ZIP); ln -s $(TARBALL_ZIP) $(TARLINK_ZIP))
	@-rm $(FINGERPRINT_GZ)
	@-rm $(FINGERPRINT_XZ)
#	@-rm $(FINGERPRINT_ZIP)

doxygen:
	@-rm -f $(HTML)/doxygen/html/*
	@(umask 022; doxygen md/vreng.dox)

snapshot:
	$(MAKE) dist distdir=$(PACKAGE_NAME)`date +"%y%m%d"`

history:
	@(cd $(ARCHIVE); ls -ltr)

rss:
	support/vreng-rss

.PHONY: dist fingerprint doxygen snapshot history rss svntags data-tar
