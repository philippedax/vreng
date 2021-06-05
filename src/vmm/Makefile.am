AUTOMAKE_OPTIONS = foreign no-dependencies

# Source files
COLLECTOR_FILES= ArgumentsParser.java Collector.java GlobalProperties.java MachineCollector.java MyStringTokenizer.java VREColor.java VREMachineWriter.java VREOutputStream.java VRERoomWriter.java VRESubnetWriter.java VREWriter.java

WORLD_FILES= Disk.java Machine.java MulticastAddress.java Pair.java Quadruplet.java Room.java Subnet.java Triplet.java World.java WorldObject.java

EXTRA_DIST = vmm vmm.conf $(WORLD_FILES) $(COLLECTOR_FILES) README AUTHORS VERSION 

if VMM

# Javadoc output directory
DOC_DIR=$(srcdir)/doc
# .class file directory
CLASS_DIR=classes
WORLD_JAR=$(CLASS_DIR)/world.jar
COLLECTOR_JAR=$(CLASS_DIR)/collector.jar
# All the "temporary" directories
DIRS=$(CLASS_DIR)
# Jar files
JARS=$(WORLD_JAR) $(COLLECTOR_JAR)
CLASSPATH=$(CLASS_DIR):$(WORLD_JAR):$(COLLECTOR_JAR)
JAVAC_FLAGS=-g -classpath $(CLASSPATH)

# Build targets
all: $(DIRS) $(JARS) vmm notice

vmm: $(JARS)
	@echo "" >/dev/null

# Create the temporary directories
$(DIRS):
	@-mkdir -p $@

if JAVA
# Compile the world classes
$(WORLD_JAR): $(WORLD_FILES)
	@echo -- Compiling world files...
	$(JAVAC) $(JAVAC_FLAGS) -d $(CLASS_DIR) $(WORLD_FILES)
	@echo -- Creating world jar...
	$(JAR) c0f $(WORLD_JAR) $(CLASS_DIR)/world/*

# Compile the collector classes
$(COLLECTOR_JAR): $(COLLECTOR_FILES)
	@echo -- Compiling collector files...
	$(JAVAC) $(JAVAC_FLAGS) -d $(CLASS_DIR) $(COLLECTOR_FILES)
	@echo -- Creating collector jar...
	$(JAR) c0f $(COLLECTOR_JAR) $(CLASS_DIR)/collector/*

# Build the javadoc for the project
javadoc: $(DIRS)
	javadoc -classpath $(CLASSPATH) -d $(DOC_DIR) \
	-doctitle "Vreng Machine Monitoring" \
	-windowtitle "Vreng Machine Monitoring" \
	-footer "Jerome Clerico - ENST - July 2001" \
	-group  "world classes" "world" \
	-group  "collector classes" "collector"
endif

install: $(JARS)
	if test ! -f $(prefix)/etc/vmm.conf; then $(INSTALL) -m 644 vmm.conf $(prefix)/etc; fi
	if test ! -d $(prefix)/classes/$(PACKAGE); then $(INSTALL) -d -m 755 $(prefix)/classes/$(PACKAGE); fi
	if test ! -d $(prefix)/classes/$(PACKAGE)/world; then $(INSTALL) -d -m 755 $(prefix)/classes/$(PACKAGE/world); fi
	if test ! -d $(prefix)/classes/$(PACKAGE)/collector; then $(INSTALL) -d -m 755 $(prefix)/classes/$(PACKAGE/collector); fi
	$(INSTALL) -m 644 $(CLASS_DIR)/world/*.class $(prefix)/classes/$(PACKAGE)/world
	$(INSTALL) -m 644 $(CLASS_DIR)/collector/*.class $(prefix)/classes/$(PACKAGE)/collector
#	$(INSTALL) -m 644 $(JARS) $(prefix)/classes/$(PACKAGE)

# Remove the temporary directories and their contents
clean:
	@-rm -rf $(DIRS) $(JARS)
	@-find . -type f -name "*~" | xargs -r -t rm -f

.PHONY: notice
notice:
	@echo -- Java libraries up to date \($(COLLECTOR_JAR)\)

endif
