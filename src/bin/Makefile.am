AUTOMAKE_OPTIONS = foreign no-dependencies

# paths for sources
TOP = $(top_srcdir)
SRC = $(top_srcdir)/src
VRENG_SRC = $(SRC)/base/vreng.cpp

# sources
vreng_SOURCES = $(VRENG_SRC)

#
# targets
#
MAIN_TARGET = all-am
OTHER_TARGETS = vrl vrel vred vacs vaps vjs vmm
all: $(MAIN_TARGET) $(OTHER_TARGETS)

# to install
bin_PROGRAMS = vreng

# to copy into bin/
if ENABLE_VRL
vrl: $(SRC)/utils/vrl/vrl
	@-cp -p $(SRC)/utils/vrl/vrl $(TOP)
else
vrl:
	@echo "" >/dev/null
endif
if ENABLE_VREL
vrel: $(SRC)/utils/vrel/vrel
	@-cp -p $(SRC)/utils/vrel/vrel $(TOP)
else
vrel:
	@echo "" >/dev/null
endif
if ENABLE_VRED
vred: $(SRC)/utils/vred/vred
	@-cp -p $(SRC)/utils/vred/vred $(TOP)
else
vred:
	@echo "" >/dev/null
endif
if ENABLE_VACS
vacs: $(SRC)/servers/vacs/vacs
	@-cp -p $(SRC)/servers/vacs/vacs $(TOP)
else
vacs:
	@echo "" >/dev/null
endif
if ENABLE_VAPS
vaps: $(SRC)/servers/vaps/vaps
	@-cp -p $(SRC)/servers/vaps/vaps $(TOP)
else
vaps:
	@echo "" >/dev/null
endif
if ENABLE_VJS
vjs: $(SRC)/servers/vjs/vjs
	@-cp -p $(SRC)/servers/vjs/vjs $(TOP)
else
vjs:
	@echo "" >/dev/null
endif
if ENABLE_VMM
vmm: $(SRC)/servers/vmm/vmm
	@-cp -p $(SRC)/servers/vmm/vmm $(TOP)
	chmod 755 vmm
else
vmm:
	@echo "" >/dev/null
endif

#
# core libs
#
BASE_LIB  = -L$(SRC)/base  $(SRC)/base/libbase.a
GUI_LIB   = -L$(SRC)/gui   $(SRC)/gui/libgui.a
WORLD_LIB = -L$(SRC)/world $(SRC)/world/libworld.a
2D_LIB    = -L$(SRC)/2d    $(SRC)/2d/lib2d.a
3D_LIB    = -L$(SRC)/3d    $(SRC)/3d/lib3d.a
NET_LIB   = -L$(SRC)/net   $(SRC)/net/libnet.a
MODEL_LIB = -L$(SRC)/model $(SRC)/model/libmodel.a
TOOL_LIB  = -L$(SRC)/tool  $(SRC)/tool/libtool.a
VNC_LIB   = -L$(SRC)/vnc   $(SRC)/vnc/libvnc.a
UBIT_LIB  = -L$(SRC)/ubit  $(SRC)/ubit/libubit.a
if ENABLE_OCAML
AI_LIB    = -L$(SRC)/ai    $(SRC)/ai/libai.a @OCAML_LIB@ -lasmrun $(SRC)/ai/libai.a -lunix -lasmrun
endif

CORE_LIBS = $(BASE_LIB) $(GUI_LIB) $(WORLD_LIB) $(MODEL_LIB) $(2D_LIB) $(3D_LIB) $(NET_LIB) $(TOOL_LIB) $(VNC_LIB) $(UBIT_LIB) $(AI_LIB)

#
# embedded libraries
#
XML_LIB   = -L$(SRC)/ext/xmlParser $(SRC)/ext/xmlParser/libxmlparser.a
MPEG_LIB  = -L$(SRC)/ext/mpeg_lib  $(SRC)/ext/mpeg_lib/libmpeg.a
FTGL_LIB  = -L$(SRC)/ext/ftgl      $(SRC)/ext/ftgl/libftgl.a
GLUI_LIB  = -L$(SRC)/ext/glui      $(SRC)/ext/glui/libglui.a
if ENABLE_GL2PS
GL2PS_LIB = -L$(SRC)/ext/gl2ps $(SRC)/ext/gl2ps/libgl2ps.a
endif
if ENABLE_TINYGL
TINYGL_LIB = -L$(SRC)/ext/TinyGL $(SRC)/ext/TinyGL/libTinyGL.a
endif

EXT_LIBS = $(XML_LIB) $(MPEG_LIB) $(FTGL_LIB) $(GLUI_LIB) $(GL2PS_LIB) $(TINYGL_LIB)

#
# executable
#
vreng_LDADD = $(CORE_LIBS) $(EXT_LIBS) -lm
