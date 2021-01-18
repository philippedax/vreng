AUTOMAKE_OPTIONS = foreign no-dependencies

EXTRA_DIST = LICENSE CHANGES

PNG_SRCS = png.c pngerror.c pnggccrd.c pngget.c pngmem.c pngpread.c pngread.c pngrio.c pngrtran.c pngrutil.c pngset.c pngtrans.c pngvcrd.c pngwio.c pngwrite.c pngwtran.c pngwutil.c
PNG_HDRS = mypng.h pngconf.h

SRCDIR = $(top_srcdir)/src
AM_CPPFLAGS = -I$(SRCDIR)/bundle/zlib -DPNG_NO_MMX_CODE -DPNG_NO_USER_MEM

if BUNDLE_LIBPNG
noinst_LIBRARIES = libpng.a
libpng_a_SOURCES = $(PNG_SRCS) $(PNG_HDRS)
endif

