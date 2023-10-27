AUTOMAKE_OPTIONS = foreign no-dependencies

EXTRA_DIST = ChangeLog README LICENSE LIMITATIONS include/*

TINYGL_SRCS = api.c arrays.c clear.c clip.c error.c get.c glx.c image_util.c init.c light.c list.c matrix.c memory.c misc.c msghandling.c nglx.c oscontext.c select.c specbuf.c texture.c vertex.c zbuffer.c zdither.c zline.c zmath.c ztriangle.c

TINYGL_HDRS = msghandling.h opinfo.h specbuf.h zbuffer.h zfeatures.h zgl.h zline.h zmath.h ztriangle.h

AM_CPPFLAGS = -I$(top_srcdir)

if AM_LIBTINYGL
noinst_LIBRARIES = libTinyGL.a
libTinyGL_a_SOURCES = $(TINYGL_SRCS) $(TINYGL_HDRS)
endif
