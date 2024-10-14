AUTOMAKE_OPTIONS = foreign

TEX_SRCS = img.cpp texture.cpp gif.cpp jpg.cpp png.cpp ppm.cpp pcx.cpp sgi.cpp txf.cpp pgm.cpp bmp.cpp tga.cpp dds.cpp tif.cpp xpm.cpp xbm.cpp psd.cpp avi.cpp svg.cpp glpng.cpp flare.cpp wheel.cpp capture.cpp

TEX_HDRS = img.hpp texture.hpp txf.hpp avi.hpp glpng.hpp flare.hpp wheel.hpp capture.hpp nanosvg.h nanosvgrast.h

SRC = $(top_srcdir)/src
AM_CPPFLAGS = @UBIT_CFLAGS@ @GL_CFLAGS@ -I$(SRC)/base -I$(SRC)/gui -I$(SRC)/world -I$(SRC)/3d -I$(SRC)/net -I$(SRC)/tool

lib2d_a_SOURCES = $(TEX_SRCS) $(TEX_HDRS)
noinst_LIBRARIES = lib2d.a
