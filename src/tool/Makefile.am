AUTOMAKE_OPTIONS = foreign

APP_SRCS = tool.cpp xterm.cpp audio.cpp video.cpp wb.cpp browser.cpp ps.cpp pdf.cpp mp3.cpp wav.cpp au.cpp midi.cpp mpeg.cpp wget.cpp modeler.cpp office.cpp sound.cpp sounds.cpp openal.cpp

APP_HDRS = tool.hpp xterm.hpp audio.hpp video.hpp wb.hpp browser.hpp ps.hpp pdf.hpp mp3.hpp wav.hpp au.hpp midi.hpp mpeg.hpp wget.hpp modeler.hpp office.hpp sound.hpp sounds.hpp openal.hpp

SRC = $(top_srcdir)/src
AM_CPPFLAGS = -I$(SRC)/base -I$(SRC)/world -I$(SRC)/net

libtool_a_SOURCES = $(APP_SRCS) $(APP_HDRS)
noinst_LIBRARIES = libtool.a
