## Makefile.am
##
## Maintainer: Philippe Dax
##
AUTOMAKE_OPTIONS = foreign no-dependencies
ACLOCAL_AMFLAGS = -I config

VRL_SRCS = vrl.cpp vre.cpp
VRL_HDRS = vrl.hpp vre.hpp global.hpp

EXTRA_DIST = vrl.l vrl.y


if ENABLE_VRL

AM_CPPFLAGS = -I$(top_srcdir)/src/base

lex.yy.o: lex.yy.cpp y.tab.h
y.tab.o: y.tab.cpp y.tab.h

lex.yy.cpp: vrl.l
	$(LEX) vrl.l
	mv lex.yy.c lex.yy.cpp
y.tab.cpp y.tab.h: vrl.y
	$(YACC) -d -y vrl.y
	mv y.tab.c y.tab.cpp

clean:
	@-rm vrl *.o lex.yy.c* y.tab.* y.output


vrl_SOURCES = $(VRL_SRCS) $(VRL_HDRS) y.tab.cpp lex.yy.cpp
bin_PROGRAMS = vrl
vrl_LDADD = -lm

endif

