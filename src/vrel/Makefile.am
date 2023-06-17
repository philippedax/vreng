AUTOMAKE_OPTIONS = foreign

MAIN_SRCS = vrel.cpp
MAIN_HDRS = vrel.hpp

ENV_SRCS = envvar.cpp namelist.cpp program.cpp tabobj.cpp tabvar.cpp
ENV_HDRS = envvar.hpp namelist.hpp program.hpp tabobj.hpp tabvar.hpp

DECL_SRCS = declvar.cpp decllist.cpp
DECL_HDRS = decl.hpp declvar.hpp decllist.hpp

INST_SRCS = affectation.cpp alternative.cpp while.cpp new.cpp instlist.cpp
INST_HDRS = affectation.hpp alternative.hpp while.hpp new.hpp inst.hpp instlist.hpp

OPER_SRCS = abs.cpp add.cpp and.cpp datalist.cpp dif.cpp div.cpp egal.cpp false.cpp inf.cpp infegal.cpp mod.cpp mul.cpp not.cpp number.cpp opp.cpp or.cpp str.cpp sub.cpp sup.cpp supegal.cpp true.cpp var.cpp # varobjet.cpp
OPER_HDRS = abs.hpp add.hpp and.hpp compare.hpp data.hpp datalist.hpp dif.hpp div.hpp egal.hpp false.hpp inf.hpp infegal.hpp mod.hpp mul.hpp not.hpp number.hpp operation.hpp logical.hpp opp.hpp or.hpp str.hpp sub.hpp sup.hpp supegal.hpp test.hpp true.hpp var.hpp # varobjet.hpp

CLASS_SRCS = aoi.cpp app.cpp walls.cpp board.cpp box.cpp button.cpp cauldron.cpp extends.cpp compound.cpp constructor.cpp doc.cpp door.cpp ground.cpp envclass.cpp envconstr.cpp envobject.cpp error.cpp gate.cpp host.cpp step.cpp wall.cpp web.cpp mirage.cpp
CLASS_HDRS = aoi.hpp app.hpp walls.hpp board.hpp box.hpp button.hpp cauldron.hpp extends.hpp class.hpp compound.hpp constructor.hpp doc.hpp door.hpp ground.hpp envclass.hpp envconstr.hpp envobject.hpp error.hpp gate.hpp host.hpp primitive.hpp step.hpp wall.hpp web.hpp mirage.hpp

VREL_SRCS = $(MAIN_SRCS) $(MAIN_HDRS) $(ENV_SRCS) $(ENV_HDRS) $(DECL_SRCS) $(DECL_HDRS) $(INST_SRCS) $(INST_HDRS) $(OPER_SRCS) $(OPER_HDRS) $(CLASS_SRCS) $(CLASS_HDRS)

EXTRA_DIST = vrel.l vrel.y


if ENABLE_VREL
lex.yy.o: lex.yy.cpp y.tab.h
y.tab.o: y.tab.cpp y.tab.h

lex.yy.cpp: vrel.l
	flex vrel.l
	mv lex.yy.c lex.yy.cpp
y.tab.cpp y.tab.h: vrel.y
	bison -d -y vrel.y
	mv y.tab.c y.tab.cpp

clean:
	@-rm vrel *.o y.output y.tab.h y.tab.cpp lex.yy.cpp

SRCDIR = $(top_srcdir)/src
AM_CPPFLAGS = -I$(SRCDIR)/vreng

vrel_SOURCES = $(VREL_SRCS) y.tab.cpp lex.yy.cpp
bin_PROGRAMS = vrel

endif
