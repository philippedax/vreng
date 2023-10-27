AUTOMAKE_OPTIONS = foreign no-dependencies

EXTRA_DIST = CHANGES doc/*

MPEG_SRCS = 24bit.c 2x2.c decoders.c fs2.c fs2fast.c fs4.c gdith.c globals.c gray.c hybrid.c hybriderr.c jrevdct.c mb_ordered.c mono.c motionvector.c ordered2.c ordered.c parseblock.c util.c video.c wrapper.c

MPEG_HDRS = decoders.h dither.h fs2.h fs4.h globals.h mpeg.h my_dmalloc.h proto.h util.h video.h

AM_CPPFLAGS = -I$(top_srcdir)

if AM_LIBMPEG
noinst_LIBRARIES = libmpeg.a
libmpeg_a_SOURCES = $(MPEG_SRCS) $(MPEG_HDRS)
endif
