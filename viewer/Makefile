# Make Viewer
CFLAGS  := $(shell pkg-config gtk4 --cflags) -Wall
LIBS    := $(shell pkg-config gtk4 --libs)
NAME    := com.github.mi19a009.PictureViewer
BIN     := ~/.local/bin
ENTRIES := ~/.local/share/applications
ICONS   := ~/.local/share/icons/hicolor/48x48/apps
SCHEMAS := ~/.local/share/glib-2.0/schemas
OUT     := .
TARGET  := .
GTK = \
	$(TARGET)/main.o \
	$(TARGET)/gresources.o
VIEWER = \
	$(TARGET)/viewerapplication.o \
	$(TARGET)/viewerapplicationwindow.o
ASSETS = \
	$(wildcard *.ui) \
	$(wildcard gtk/*.ui) \
	$(wildcard icons/48x48/actions/*.png)
.PHONY: build debug release
all: build/viewer
build: build/viewer
debug: debug/viewer
release: release/viewer
schemas: gschemas.compiled
clean:
	rm -rf build debug release $(wildcard gresources.*) $(wildcard gschemas.*)
install:
	@$(MAKE) $(BIN)/$(NAME)
	@$(MAKE) $(ENTRIES)/$(NAME).desktop
	@$(MAKE) $(ICONS)/$(NAME).png
	@$(MAKE) $(SCHEMAS)/$(NAME).gschema.xml
	@$(MAKE) $(SCHEMAS)/gschemas.compiled
uninst:
	$(RM) $(BIN)/$(NAME)
	$(RM) $(ENTRIES)/$(NAME).desktop
	$(RM) $(ICONS)/$(NAME).png
	$(RM) $(SCHEMAS)/$(NAME).gschema.xml
	glib-compile-schemas $(SCHEMAS)
# Targets
build/viewer:
	@$(MAKE) $@ OUT=$@ TARGET=build
debug/viewer:
	@$(MAKE) $@ OUT=$@ TARGET=debug "CFLAGS = $(CFLAGS) -g -DG_ENABLE_DEBUG"
release/viewer:
	@$(MAKE) $@ OUT=$@ TARGET=release "CFLAGS = $(CFLAGS) -O2 -DNDEBUG -DG_DISABLE_ASSERT -DG_DISABLE_CAST_CHECKS"
# Sources
gresources.c: viewer.gresources.xml $(ASSETS)
	@echo $@
	@glib-compile-resources $< --target $@ --generate-source
gresources.h: viewer.gresources.xml
	@echo $@
	@glib-compile-resources $< --target $@ --generate-header
$(GTK): $(TARGET)/%.o: %.c
	@echo $@
	@mkdir -p $(TARGET)
	@$(CC) $(CFLAGS) -c -o $@ $<
$(VIEWER): $(TARGET)/%.o: %.c viewer.h
	@echo $@
	@mkdir -p $(TARGET)
	@$(CC) $(CFLAGS) -c -o $@ $<
$(OUT): $(GTK) $(VIEWER)
	@echo $@
	@$(CC) $(CFLAGS) -o $@ $(GTK) $(VIEWER) $(LIBS)
# Binaries
$(BIN)/$(NAME): release/viewer
	@echo $@
	@mkdir -p $(BIN)
	@cp $< $@
# Desktop Entries
$(ENTRIES)/$(NAME).desktop: viewer.desktop
	@echo $@
	@mkdir -p $(ENTRIES)
	@cp $< $@
# Icons
$(ICONS)/$(NAME).png: icons/48x48/actions/viewer.png
	@echo $@
	@mkdir -p $(ICONS)
	@cp $< $@
# Schemas
gschemas.valid: viewer.gschema.xml
	@echo $@
	@glib-compile-schemas --strict --dry-run --schema-file=$<
	@touch $@
gschemas.compiled: gschemas.valid
	@echo $@
	@glib-compile-schemas .
$(SCHEMAS)/$(NAME).gschema.xml: viewer.gschema.xml gschemas.valid
	@echo $@
	@mkdir -p $(SCHEMAS)
	@cp $< $@
$(SCHEMAS)/gschemas.compiled:
	@echo $@
	@glib-compile-schemas $(SCHEMAS)
