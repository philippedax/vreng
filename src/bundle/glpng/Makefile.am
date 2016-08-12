AUTOMAKE_OPTIONS = foreign no-dependencies

GLPNG_SRCS = glpng.c glpng.h

SRCDIR = $(top_srcdir)/src
if BUNDLE_LIBPNG
AM_CPPFLAGS = -I$(SRCDIR)/vgl -I$(SRCDIR)/bundle/png -I$(SRCDIR)/bundle/zlib @GL_CFLAGS@
else
AM_CPPFLAGS = -I$(SRCDIR)/vgl -I$(SRCDIR)/bundle/png @GL_CFLAGS@
endif

noinst_LIBRARIES = libglpng.a
libglpng_a_SOURCES = $(GLPNG_SRCS)

