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
if ENABLE_JAVA

bin_SCRIPTS = vjs

# ---------------
# Global settings
# ---------------
# Vreng directory
VRENG_DIR=@top_srcdir@
# Vreng Java directory
VJS_DIR=@srcdir@
# Install directory
INSTALL_DIR=/Users/dax/vreng/htdocs/jar

# ------------------
# Source directories
# ------------------
# Generated source directory
GEN_DIR=$(VJS_DIR)/gen
# Base and server source directory
BASE_DIR=$(VJS_DIR)/base
# Applications source directories
APPS_DIR=$(VJS_DIR)/app
# Jar directory
JAR_DIR=$(VJS_DIR)/jar

# Various source subdirectories
SRC_DIR=$(BASE_DIR)/fr/enst/vreng
DATA_DIR=$(BASE_DIR)/fr/enst/vreng/data
SERVER_DIR=$(BASE_DIR)/fr/enst/vreng/server
NET_DIR=$(BASE_DIR)/fr/enst/vreng/net
APP_DIR=$(APPS_DIR)/fr/enst/vreng/tester

# ------------------
# Output directories
# ------------------
# .class file base directory
CLASSES_DIR=$(VJS_DIR)/classes
# Generated .class files directory
GEN_CLASS_DIR=$(CLASSES_DIR)/gen
# Base .class files directory
BASE_CLASS_DIR=$(CLASSES_DIR)/base
# Server .class files directory
SERVER_CLASS_DIR=$(CLASSES_DIR)/server
# Applications .class files directory
APPS_CLASS_DIR=$(CLASSES_DIR)/app
# All the "temporary" directories
TMPS=$(LIB_DIR) $(CLASSES_DIR) $(GEN_CLASS_DIR) $(APPS_CLASS_DIR) $(BASE_CLASS_DIR) $(SERVER_CLASS_DIR) $(JAR_DIR)
# Jar files directory
LIB_DIR=$(VJS_DIR)/lib
# Javadoc output directory
DOC_DIR=$(VJS_DIR)/doc

# ---------
# Jar files
# ---------
# Final jar files
CORE_JAR=$(JAR_DIR)/vrengcore.jar
APP_JAR=$(JAR_DIR)/vrengapp.jar

# Intermediate jar files
GEN_JAR=$(LIB_DIR)/gen.jar
BASE_JAR=$(LIB_DIR)/base.jar
SERVER_JAR=$(LIB_DIR)/server.jar

# Files that need to be built
JARS=$(CORE_JAR) $(APP_JAR)

# -------------
# Java settings
# -------------
# Set classpath and flags
CLASSPATH=$(GEN_CLASS_DIR):$(BASE_CLASS_DIR):$(SERVER_CLASS_DIR):$(APPS_CLASS_DIR):$(GEN_JAR):$(BASE_JAR):$(APP_JAR)
JAVAC_FLAGS=-g -classpath $(CLASSPATH) -Xlint:-unchecked -Xlint:deprecation

# ------------
# Source files
# ------------
# - source dependencies
GEN_DEP=\
	$(VRENG_DIR)/src/wo/vjc.hpp \
	$(VRENG_DIR)/src/wo/objects.cpp

# - generated source
GEN_FILES=\
	$(GEN_DIR)/fr/enst/vreng/MessageTypes.java \
	$(GEN_DIR)/fr/enst/vreng/ObjectTypes.java

# Application files
APP_FILES=\
        $(APP_DIR)/Fan.java \
        $(APP_DIR)/Slide.java \
        $(APP_DIR)/Board.java \
        $(APP_DIR)/TicTacToe.java \
        $(APP_DIR)/Ring.java

# Base files
BASE_FILES=\
	$(DATA_DIR)/types/Point2D.java \
	$(DATA_DIR)/types/Point3D.java \
	$(DATA_DIR)/types/ObjPos.java \
	$(DATA_DIR)/types/NetObjId.java \
	$(DATA_DIR)/messages/PosMessage.java \
	$(DATA_DIR)/messages/DrawMessage.java \
	$(DATA_DIR)/messages/ClickMessage.java \
	$(DATA_DIR)/messages/QueryMessage.java \
	$(DATA_DIR)/messages/ControlMessage.java \
	$(DATA_DIR)/messages/IntersectMessage.java \
	$(DATA_DIR)/Header.java \
	$(DATA_DIR)/Message.java \
	$(DATA_DIR)/DataHelper.java \
	$(DATA_DIR)/MsgInputStream.java \
	$(DATA_DIR)/MsgOutputStream.java \
	$(DATA_DIR)/InvalidMessageException.java \
	$(SRC_DIR)/IMessage.java \
	$(SRC_DIR)/IVrengApp.java \
	$(SRC_DIR)/IVrengObject.java \
	$(SRC_DIR)/IVrengProcessor.java \
	$(SRC_DIR)/VrengException.java

# Server files
SERVER_FILES=\
	$(SERVER_DIR)/Vreng.java \
	$(SERVER_DIR)/VrengApp.java \
	$(SERVER_DIR)/VrengObject.java \
	$(SERVER_DIR)/Server.java \
	$(SERVER_DIR)/AppController.java \
	$(SERVER_DIR)/ClientController.java \
	$(NET_DIR)/Sender.java \
	$(NET_DIR)/Receiver.java

# -------------
# Build targets
# -------------
all: $(TMPS) $(JARS) # notice

# Create the temporary directories
$(TMPS):
	mkdir -p $@

# Generate the constants files
$(GEN_FILES): $(GEN_DEP)
	@echo -- Generating constants files...
	perl gen.pl $(GEN_DEP) fr.enst.vreng MessageTypes ObjectTypes $(GEN_DIR)

# Compile the constants files
$(GEN_JAR): $(GEN_FILES)
	@echo -- Compiling constants files...
	javac $(JAVAC_FLAGS) -d $(GEN_CLASS_DIR) $(GEN_FILES)
	-jar c0f $(GEN_JAR) -C $(GEN_CLASS_DIR) fr

# Build the core jar file
$(CORE_JAR): $(GEN_JAR) $(BASE_JAR) $(SERVER_JAR)
	@echo -- Creating Vreng server core jar...
	-jar c0f $(CORE_JAR) -C $(GEN_CLASS_DIR) fr
	-jar u0f $(CORE_JAR) -C $(BASE_CLASS_DIR) fr
	-jar u0f $(CORE_JAR) -C $(SERVER_CLASS_DIR) fr
	-@chmod 755 $(bin_SCRIPTS)

# Compile the base classes
$(BASE_JAR): $(GEN_JAR) $(BASE_FILES)
	javac $(JAVAC_FLAGS) -d $(BASE_CLASS_DIR) $(BASE_FILES)
	-jar c0f $(BASE_JAR) -C $(BASE_CLASS_DIR) fr

# Compile the server classes
$(SERVER_JAR): $(GEN_JAR) $(SERVER_FILES)
	javac $(JAVAC_FLAGS) -d $(SERVER_CLASS_DIR) $(SERVER_FILES)
	-jar c0f $(SERVER_JAR) -C $(SERVER_CLASS_DIR) fr

# Compile the application classes
$(APP_JAR): $(GEN_JAR) $(APP_FILES)
	javac $(JAVAC_FLAGS) -d $(APPS_CLASS_DIR) $(APP_FILES)
	-jar c0f $(APP_JAR) -C $(APPS_CLASS_DIR) fr

# Remove the temporary directories and their contents
clean:
	-rm -rf $(TMPS)

# Like clean, but removes the Makefile too (plus backup files)
clean-dist: clean
	-@BACKUP=`find . -type f -name "*~"` \
	&& if test ! -z "$$BACKUP" ; then \
		rm -f $$BACKUP ; \
	fi
	-rm -f Makefile

# Build the javadoc for the project
javadoc: $(TMPS)
	$(JAVADOC) \
	-classpath $(CLASSPATH) \
	-d $(DOC_DIR) \
	-sourcepath $(BASE_DIR):$(APP_DIR):$(GEN_DIR) \
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
	-cp vjs $(DESTDIR)$(bindir)
	-cp -p $(JARS) $(INSTALL_DIR)

.PHONY: notice
notice:
	@echo -- vjs up to date

endif # ENABLE_JAVA
endif # ENABLE_VJS
