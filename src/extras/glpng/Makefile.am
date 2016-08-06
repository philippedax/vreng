AUTOMAKE_OPTIONS = foreign no-dependencies

GLPNG_SRCS = glpng.c glpng.h

SRCDIR = $(top_srcdir)/src
if MYLIBPNG
AM_CPPFLAGS = -I$(SRCDIR)/vgl -I$(SRCDIR)/extras/png -I$(SRCDIR)/extras/zlib @GL_CFLAGS@
else
AM_CPPFLAGS = -I$(SRCDIR)/vgl -I$(SRCDIR)/extras/png @GL_CFLAGS@
endif

noinst_LIBRARIES = libglpng.a
libglpng_a_SOURCES = $(GLPNG_SRCS)

