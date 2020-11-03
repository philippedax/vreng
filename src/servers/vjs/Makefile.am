## Process this file with automake to create Makefile.in

## Makefile for Automake.

## Copyright (C) 1995, 1996, 1997, 1998, 1999, 2001, 2002, 2003, 2004
## Free Software Foundation, Inc.

## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.

## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.

## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

AUTOMAKE_OPTIONS = foreign no-dependencies

SUBDIRS = app base

EXTRA_DIST = gen.pl vjs.in

if ENABLE_VJS
if JAVA

bin_SCRIPTS = vjs

# ---------------
# Global settings
# ---------------
# Vreng directory
VRENG_DIR=@top_srcdir@
# Vreng Java directory
VJS_DIR=@srcdir@
# Install directory
#INSTALL_DIR=/infres/ir500/adm/projets/vreng/jar
INSTALL_DIR=/Users/dax/vreng/data/jar

# ------------------
# Source directories
# ------------------
# Generated source directory
GEN_SUBDIR=$(VJS_DIR)/gen
# Base and server source directory
BASE_SUBDIR=$(VJS_DIR)/base
# Applications source directories
APP_SUBDIR=$(VJS_DIR)/app
# Jar directory
JAR_SUBDIR=$(VJS_DIR)/jar

# Various source subdirectories
SRC_SUBDIR=$(BASE_SUBDIR)/fr/enst/vreng
DATA_SUBDIR=$(BASE_SUBDIR)/fr/enst/vreng/data
SERVER_SUBDIR=$(BASE_SUBDIR)/fr/enst/vreng/server
NET_SUBDIR=$(BASE_SUBDIR)/fr/enst/vreng/net
APPS_SUBDIR=$(APP_SUBDIR)/fr/enst/vreng/tester

# ------------------
# Output directories
# ------------------
# Javadoc output directory
DOC_SUBDIR=$(VJS_DIR)/doc
# Jar files directory
LIB_SUBDIR=$(VJS_DIR)/lib
# .class file base directory
CLASSES_SUBDIR=$(VJS_DIR)/classes
# Generated .class files directory
GEN_CLASS_SUBDIR=$(CLASSES_SUBDIR)/gen
# Base .class files directory
BASE_CLASS_SUBDIR=$(CLASSES_SUBDIR)/base
# Server .class files directory
SERVER_CLASS_SUBDIR=$(CLASSES_SUBDIR)/server
# Applications .class files directory
APPS_CLASS_SUBDIR=$(CLASSES_SUBDIR)/app

# All the "temporary" directories
TMPDIRS=$(LIB_SUBDIR) $(CLASSES_SUBDIR) $(GEN_CLASS_SUBDIR) $(APPS_CLASS_SUBDIR) $(BASE_CLASS_SUBDIR) $(SERVER_CLASS_SUBDIR) $(JAR_SUBDIR) $(DOC_SUBDIR)

# ---------
# Jar files
# ---------
# Final jar files
VRENGCORE_JAR=$(JAR_SUBDIR)/vrengcore.jar
VRENGAPP_JAR=$(JAR_SUBDIR)/vrengapp.jar

# Intermediate jar files
GEN_JAR=$(LIB_SUBDIR)/gen.jar
BASE_JAR=$(LIB_SUBDIR)/base.jar
SERVER_JAR=$(LIB_SUBDIR)/server.jar

# Files that need to be built
JARS=$(VRENGCORE_JAR) $(VRENGAPP_JAR)

# -------------
# Java settings
# -------------
# Set classpath and flags
CLASSPATH=$(GEN_CLASS_SUBDIR):$(BASE_CLASS_SUBDIR):$(SERVER_CLASS_SUBDIR):$(APPS_CLASS_SUBDIR):$(GEN_JAR):$(BASE_JAR):$(VRENGAPP_JAR)
JAVAC_FLAGS=-g -classpath $(CLASSPATH) -Xlint:-unchecked

# ------------
# Source files
# ------------
# - source dependencies
GEN_DEP=\
	$(VRENG_DIR)/src/wo/vjc.hpp \
	$(VRENG_DIR)/src/wo/initobj.cpp

# - generated source
GEN_FILES=\
	$(GEN_SUBDIR)/fr/enst/vreng/MessageTypes.java \
	$(GEN_SUBDIR)/fr/enst/vreng/ObjectTypes.java

# Application files
APPS_FILES=\
        $(APPS_SUBDIR)/Fan.java \
        $(APPS_SUBDIR)/Ring.java \
        $(APPS_SUBDIR)/Slide.java \
        $(APPS_SUBDIR)/Board.java \
        $(APPS_SUBDIR)/TicTacToe.java

# Base files
BASE_FILES=\
	$(DATA_SUBDIR)/types/Point2D.java \
	$(DATA_SUBDIR)/types/Point3D.java \
	$(DATA_SUBDIR)/types/ObjPos.java \
	$(DATA_SUBDIR)/types/NetObjId.java \
	$(DATA_SUBDIR)/messages/PosMessage.java \
	$(DATA_SUBDIR)/messages/DrawMessage.java \
	$(DATA_SUBDIR)/messages/ClickMessage.java \
	$(DATA_SUBDIR)/messages/QueryMessage.java \
	$(DATA_SUBDIR)/messages/ControlMessage.java \
	$(DATA_SUBDIR)/messages/IntersectMessage.java \
	$(DATA_SUBDIR)/Header.java \
	$(DATA_SUBDIR)/Message.java \
	$(DATA_SUBDIR)/DataHelper.java \
	$(DATA_SUBDIR)/MsgInputStream.java \
	$(DATA_SUBDIR)/MsgOutputStream.java \
	$(DATA_SUBDIR)/InvalidMessageException.java \
	$(SRC_SUBDIR)/IMessage.java \
	$(SRC_SUBDIR)/IVrengApp.java \
	$(SRC_SUBDIR)/IVrengObject.java \
	$(SRC_SUBDIR)/IVrengProcessor.java \
	$(SRC_SUBDIR)/VrengException.java

# Server files
SERVER_FILES=\
	$(SERVER_SUBDIR)/Vreng.java \
	$(SERVER_SUBDIR)/VrengApp.java \
	$(SERVER_SUBDIR)/VrengObject.java \
	$(SERVER_SUBDIR)/Server.java \
	$(SERVER_SUBDIR)/AppController.java \
	$(SERVER_SUBDIR)/ClientController.java \
	$(NET_SUBDIR)/Sender.java \
	$(NET_SUBDIR)/Receiver.java

# -------------
# Build targets
# -------------
all: $(TMPDIRS) $(JARS) notice

# Create the temporary directories
$(TMPDIRS):
	mkdir -p $@

# Remove the temporary directories and their contents
clean:
	-rm -rf $(TMPDIRS)

# Like clean, but removes the Makefile too (plus backup files)
clean-dist: clean
	-@BACKUP=`find . -type f -name "*~"` \
	&& if test ! -z "$$BACKUP" ; then \
		rm -f $$BACKUP ; \
	fi
	-rm -f Makefile

# Generate the constants files
$(GEN_FILES): $(GEN_DEP)
	@echo -- Generating constants files...
	perl gen.pl $(GEN_DEP) fr.enst.vreng MessageTypes ObjectTypes $(GEN_SUBDIR)

# Compile the constants files
$(GEN_JAR): $(GEN_FILES)
	@echo -- Compiling constants files...
	javac $(JAVAC_FLAGS) -d $(GEN_CLASS_SUBDIR) $(GEN_FILES)
	jar c0f $(GEN_JAR) -C $(GEN_CLASS_SUBDIR) fr

# Build the core jar file
$(VRENGCORE_JAR): $(GEN_JAR) $(BASE_JAR) $(SERVER_JAR)
	@echo -- Creating Vreng server core jar...
	-jar c0f $(VRENGCORE_JAR) -C $(GEN_CLASS_SUBDIR) fr
	-jar u0f $(VRENGCORE_JAR) -C $(BASE_CLASS_SUBDIR) fr
	-jar u0f $(VRENGCORE_JAR) -C $(SERVER_CLASS_SUBDIR) fr
	-@chmod 755 $(bin_SCRIPTS)

# Compile the base classes
$(BASE_JAR): $(GEN_JAR) $(BASE_FILES)
	javac $(JAVAC_FLAGS) -d $(BASE_CLASS_SUBDIR) $(BASE_FILES)
	-jar c0f $(BASE_JAR) -C $(BASE_CLASS_SUBDIR) fr

# Compile the server classes
$(SERVER_JAR): $(GEN_JAR) $(SERVER_FILES)
	javac $(JAVAC_FLAGS) -d $(SERVER_CLASS_SUBDIR) $(SERVER_FILES)
	-jar c0f $(SERVER_JAR) -C $(SERVER_CLASS_SUBDIR) fr

# Compile the application classes
$(VRENGAPP_JAR): $(GEN_JAR) $(APPS_FILES)
	javac $(JAVAC_FLAGS) -d $(APPS_CLASS_SUBDIR) $(APPS_FILES)
	-jar c0f $(VRENGAPP_JAR) -C $(APPS_CLASS_SUBDIR) fr

# Build the javadoc for the project
javadoc: $(TMPDIRS)
	$(JAVADOC) \
	-classpath $(CLASSPATH) \
	-d $(DOC_SUBDIR) \
	-sourcepath $(BASE_SUBDIR):$(APP_SUBDIR):$(GEN_SUBDIR) \
	-doctitle "Vreng Java Server" \
	-windowtitle "Vreng Java Server" \
	-footer "Mathieu Seigneurin - ENST - July 2001" \
	-group  "Base classes" "fr.enst.vreng.data:fr.enst.vreng" \
	-group  "Data and messages" "fr.enst.vreng.data.types:fr.enst.vreng.data.messages" \
	-group  "Server and network classes" "fr.enst.vreng.net:fr.enst.vreng.server" \
	-group  "Applications" "fr.enst.vreng.tester" \
	fr.enst.vreng.data.types \
	fr.enst.vreng.data.messages \
	fr.enst.vreng.data \
	fr.enst.vreng \
	fr.enst.vreng.net \
	fr.enst.vreng.server \
	fr.enst.vreng.tester

install:
	cp vjs $(DESTDIR)$(bindir)
	cp -p $(JARS) $(INSTALL_DIR)

.PHONY: notice
notice:
	@echo -- Jar up to date \($(VRENGCORE_JAR) and $(VRENGAPP_JAR)\)

endif
endif
