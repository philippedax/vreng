AUTOMAKE_OPTIONS = foreign no-dependencies

EXTRA_DIST = README

ZLIB_SRCS = adler32.c crc32.c deflate.c gzio.c infback.c inffast.c inflate.c inftrees.c uncompr.c zutil.c
ZLIB_HDRS = crc32.h deflate.h inffast.h inffixed.h inflate.h inftrees.h trees.h zconf.h zlib.h zutil.h

AM_CPPFLAGS = -I.

if MYLIBZ
noinst_LIBRARIES = libz.a
libz_a_SOURCES = $(ZLIB_SRCS) $(ZLIB_HDRS)
endif

