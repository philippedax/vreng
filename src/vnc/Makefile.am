AUTOMAKE_OPTIONS = foreign

VNC_SRCS = vnccli.cpp vncsoc.cpp vncrfb.cpp vncauth.cpp vncdes.cpp

VNC_HDRS = vnccli.hpp vncsoc.hpp vncrfb.hpp vncauth.hpp vncdes.hpp

SRCDIR = $(top_srcdir)/src
AM_CPPFLAGS =  @UBIT_CFLAGS@ $(X_CFLAGS) -I$(SRCDIR)/sys -I$(SRCDIR)/vreng -I$(SRCDIR)/gui -I$(SRCDIR)/net

libvnc_a_SOURCES = $(VNC_SRCS) $(VNC_HDRS)
noinst_LIBRARIES = libvnc.a
