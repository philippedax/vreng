## Makefile.am
##
## Maintainer: Philippe Dax
##
AUTOMAKE_OPTIONS = foreign no-dependencies
ACLOCAL_AMFLAGS = -I config

SUBDIRS = src lib config doc examples

EXTRA_DIST = COPYRIGHT VERSION vrl.dox

## local targets

FTP_ARCHIVE = /enst/pub/vreng/$(PACKAGE)
TARBALL = $(PACKAGE)-$(VERSION).tar.gz
TARLINK = $(PACKAGE).tar.gz
FINGERPRINT = $(PACKAGE)-$(VERSION).md5sum

clean:
	@(cd src; make clean)

fingerprint:
	md5sum < $(TARBALL) > $(FINGERPRINT)

archive: fingerprint
	install -m 444 $(FINGERPRINT) $(FTP_ARCHIVE)
	install -m 444 $(TARBALL) $(FTP_ARCHIVE)
	(cd $(FTP_ARCHIVE); rm -f $(TARLINK); ln -s $(TARBALL) $(TARLINK))

history:
	@(cd $(FTP_ARCHIVE); ls -ltr)

doc:
	doxygen vrl.dox

.PHONY: fingerprint history doc
