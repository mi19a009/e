# Make Viewer
CFLAGS  := $(shell pkg-config gtk4 --cflags) -Wall
LIBS    := $(shell pkg-config gtk4 --libs)
NAME    := com.github.mi19a009.PictureViewer
BIN     := $(HOME)/.local/bin
ENTRIES := $(HOME)/.local/share/applications
ICONS   := $(HOME)/.local/share/icons/hicolor/48x48/apps
SCHEMAS := $(HOME)/.local/share/glib-2.0/schemas
OUT     := build/viewer
TARGET  := build
GTK = \
	$(TARGET)/main.o \
	$(TARGET)/gresources.o
VIEWER = \
	$(TARGET)/viewer.o \
	$(TARGET)/viewerapplication.o \
	$(TARGET)/viewerapplicationwindow.o
ASSETS = \
	$(wildcard *.ui) \
	$(wildcard gtk/*.ui) \
	$(wildcard icons/48x48/actions/*.png)
.PHONY: build debug release
all: $(OUT)
schemas: gschemas.compiled
clean:
	rm -rf build debug release $(wildcard gresources.*) $(wildcard gschemas.*)
install: \
	$(BIN)/$(NAME) \
	$(ENTRIES)/$(NAME).desktop \
	$(ICONS)/$(NAME).png \
	$(SCHEMAS)/$(NAME).gschema.xml \
	$(SCHEMAS)/gschemas.compiled
uninst:
	$(RM) $(BIN)/$(NAME)
	$(RM) $(ENTRIES)/$(NAME).desktop
	$(RM) $(ICONS)/$(NAME).png
	$(RM) $(SCHEMAS)/$(NAME).gschema.xml
	glib-compile-schemas $(SCHEMAS)
# Targets
debug:
	@$(MAKE) OUT=$@/viewer TARGET=debug "CFLAGS = $(CFLAGS) -g -DG_ENABLE_DEBUG"
release:
	@$(MAKE) OUT=$@/viewer TARGET=release "CFLAGS = $(CFLAGS) -O2 -DNDEBUG -DG_DISABLE_ASSERT -DG_DISABLE_CAST_CHECKS -DGETTEXT_PATH=\\\"$(HOME)/.local/share/locale\\\""
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
$(BIN)/$(NAME): release
	@echo $@
	@mkdir -p $(BIN)
	@cp $</viewer $@
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
