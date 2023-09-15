AUTOMAKE_OPTIONS = foreign no-dependencies

GL2PS_SRCS = gl2ps.c gl2ps.h

AM_CPPFLAGS = @GL_CFLAGS@ $(X_CFLAGS)

EXTRA_DIST = COPYING.GL2PS

if ENABLE_GL2PS
noinst_LIBRARIES = libgl2ps.a
libgl2ps_a_SOURCES = $(GL2PS_SRCS)
endif
