AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = gestures.hpp man.data

MODEL_SRCS = md2.cpp lwo.cpp off.cpp 3ds.cpp ase.cpp man.cpp obj.cpp dxf.cpp x3d.cpp v3d.cpp vec3.cpp body.cpp face.cpp bap.cpp fap.cpp bvh.cpp bone.cpp car.cpp teapot.cpp

MODEL_HDRS = md2.hpp lwo.hpp off.hpp 3ds.hpp ase.hpp man.hpp obj.hpp dxf.hpp x3d.hpp v3d.hpp vec3.hpp body.hpp face.hpp bap.hpp fap.hpp bvh.hpp bone.hpp car.hpp teapot.hpp

SRC = $(top_srcdir)/src
CFLAGS = @UBIT_CFLAGS@ @GL_CFLAGS@
AM_CPPFLAGS = $(CFLAGS) -I$(SRC)/base -I$(SRC)/gui -I$(SRC)/world -I$(SRC)/net -I$(SRC)/2d -I$(SRC)/3d -I$(SRC)/ext/xmlParser

libmodel_a_SOURCES = $(MODEL_SRCS) $(MODEL_HDRS)
noinst_LIBRARIES = libmodel.a
