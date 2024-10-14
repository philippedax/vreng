AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = glu.hpp glu.cpp

VGL_SRCS = solid.cpp draw.cpp render.cpp camera.cpp ogl.cpp axis.cpp grid.cpp

VGL_HDRS = solid.hpp draw.hpp render.hpp ogl.hpp axis.hpp grid.hpp

SRC = $(top_srcdir)/src
AM_CPPFLAGS = @UBIT_CFLAGS@ @GL_CFLAGS@ -I$(SRC)/base -I$(SRC)/gui -I$(SRC)/world -I$(SRC)/net -I$(SRC)/2d -I$(SRC)/model -I$(SRC)/ai

lib3d_a_SOURCES = $(VGL_SRCS) $(VGL_HDRS)
noinst_LIBRARIES = lib3d.a
