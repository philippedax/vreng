#!smake
#
# Copyright (c) Mark J. Kilgard, 1996, 1997.
#

CC=gcc

#DAX TOP = ../../..
#DAX include $(TOP)/glutdefs
#DAX include $(ROOT)/usr/include/make/commondefs
#DAX 
#DAX LN = ln -s
#DAX MV = mv
#DAX RM = -rm -rf
#DAX 
#DAX TARGETS = glflare
#DAX 
#DAX LLDLIBS = $(GLUT) -lGLU -lGL -lXmu -lXext -lX11 -lm
LIBS = -L/usr/local/lib -lglust -I/usr/openwin/lib -lGLU -lGL -lXmu -lXext -lX11 -lm

SRCS = glflare.c loadlum.c
OBJS =  $(SRCS:.c=.o)

#DAX LCOPTS = #DAX -I$(TOP)/include -fullwarn
#DAX LWOFF = ,813,852,827,826
#DAX LDIRT = *~ *.bak *.pure
#DAX 
#DAX default : $(TARGETS)

glflare : $(OBJS) $(LIBS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

glflare.o: glflare.c vec3d.c loadlum.h
	$(CC) -I/usr/local/include -I/usr/openwin/include $(CFLAGS) glflare.c

#DAX include $(COMMONRULES)
