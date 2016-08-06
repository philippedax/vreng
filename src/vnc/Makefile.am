AUTOMAKE_OPTIONS = foreign

VNC_SRCS = client.cpp sockets.cpp rfbproto.cpp auth.cpp d3des.cpp

VNC_HDRS = client.hpp sockets.hpp rfbproto.hpp auth.h d3des.h

SRCDIR = $(top_srcdir)/src
AM_CPPFLAGS =  @UBIT_CFLAGS@ $(X_CFLAGS) -I$(SRCDIR)/sys -I$(SRCDIR)/vreng -I$(SRCDIR)/gui -I$(SRCDIR)/net

libvnc_a_SOURCES = $(VNC_SRCS) $(VNC_HDRS)
noinst_LIBRARIES = libvnc.a
