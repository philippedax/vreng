VRED_SRCS = main.cpp app.cpp bbox.cpp board.cpp box.cpp bsphere.cpp camera.cpp collide.cpp color.cpp doc.cpp earth.cpp gate.cpp gif.cpp group.cpp host.cpp mirage.cpp thing.cpp io.cpp item.cpp map.cpp matrix.cpp output.cpp ray.cpp raycaster.cpp solid.cpp sphere.cpp step.cpp tex.cpp vect.cpp safe.cpp wall.cpp web.cpp ui.cpp

VRED_HDRS = main.hpp vred.hpp vre.hpp app.hpp bbox.hpp board.hpp box.hpp bsphere.hpp camera.hpp color.hpp doc.hpp earth.hpp gate.hpp gif.hpp group.hpp host.hpp mirage.hpp thing.hpp input.hpp item.hpp map.hpp matrix.hpp output.hpp ray.hpp raycaster.hpp solid.hpp sphere.hpp step.hpp tex.hpp ui.hpp vect.hpp safe.hpp wall.hpp web.hpp

EXTRA_DIST = vred.l vred.y

SRCDIR = $(top_srcdir)/src
AM_CPPFLAGS = -I$(SRCDIR)/vreng -I$(SRCDIR)/ext/glui $(GL_CFLAGS) $(X_CFLAGS)

if ENABLE_VRED
lex.yy.o: lex.yy.cpp y.tab.h
y.tab.o: y.tab.cpp y.tab.h

lex.yy.cpp: vred.l
	$(LEX) vred.l
	mv lex.yy.c /tmp/lex.yy.cpp
	sed -e "s%^int yylineno%//int yylineno%" </tmp/lex.yy.cpp >lex.yy.cpp
	rm /tmp/lex.yy.cpp
y.tab.cpp y.tab.h: vred.y
	bison -d -y vred.y
	mv y.tab.c y.tab.cpp

clean:
	@-rm vred *.o y.output y.tab.h y.tab.cpp lex.yy.cpp


vred_SOURCES = $(VRED_SRCS) $(VRED_HDRS) y.tab.cpp lex.yy.cpp

bin_PROGRAMS = vred

MACOS_GLUT_LIB = -L/System/Library/Frameworks -framework GLUT -framework OpenGL
MACOS_GLUT = /System/Library/Frameworks/GLUT.framework/Versions/A/GLUT
MACOS_FREEGLUT_LIB = -L/usr/local/opt/freeglut/lib -lglut 

vred_LDADD = -L$(SRCDIR)/ext/glui $(SRCDIR)/ext/glui/libglui.a -lglut -L/usr/X11/lib -lGLU -lGL -lX11 -lm 

endif
