AUTOMAKE_OPTIONS = foreign

NET_SRCS = event.cpp channel.cpp netobj.cpp noid.cpp payload.cpp netprop.cpp rtp.cpp session.cpp source.cpp sap.cpp md5.cpp vac.cpp

NET_HDRS = vrep.hpp event.hpp channel.hpp netobj.hpp noid.hpp payload.hpp netprop.hpp rtp.hpp session.hpp source.hpp sap.hpp md5.hpp vac.hpp

SRC = $(top_srcdir)/src
AM_CPPFLAGS = -I$(SRC)/base -I$(SRC)/gui -I$(SRC)/world -I$(SRC)/2d -I$(SRC)/3d -I$(SRC)/tool -I$(SRC)

libnet_a_SOURCES = $(NET_SRCS) $(NET_HDRS)
noinst_LIBRARIES = libnet.a
