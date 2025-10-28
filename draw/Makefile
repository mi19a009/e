# Make Draw
CFLAGS  := $(shell pkg-config gtk4 --cflags) -Wall
LIBS    := $(shell pkg-config gtk4 --libs)
TARGET  := build
OUT      = ../$(TARGET)/draw
OBJ      = $(TARGET)/gresources.o
DRAW = \
	$(TARGET)/drawing.o \
	$(TARGET)/drawingapplication.o \
	$(TARGET)/drawingapplicationwindow.o
ASSETS = \
	$(wildcard *.ui) \
	$(wildcard gtk/*.ui) \
	$(wildcard icons/48x48/actions/*.png)
.PHONY: all build debug release
all: $(OUT)
clean:
	rm -rf build debug release $(wildcard gresources.*)
debug:
	@$(MAKE) TARGET=$@ "CFLAGS = $(CFLAGS) -g -DG_ENABLE_DEBUG"
release:
	@$(MAKE) TARGET=$@ "CFLAGS = $(CFLAGS) -O2 -DNDEBUG -DG_DISABLE_ASSERT -DG_DISABLE_CAST_CHECKS -DGETTEXT_PATH=\\\"$(HOME)/.local/share/locale\\\""
gresources.c: drawing.gresources.xml $(ASSETS)
	@echo $@
	@glib-compile-resources $< --target $@ --generate-source
$(OBJ): $(TARGET)/%.o: %.c
	@echo $@
	@mkdir -p $(TARGET)
	@$(CC) $(CFLAGS) -c -o $@ $<
$(DRAW): $(TARGET)/%.o: %.c drawing.h
	@echo $@
	@mkdir -p $(TARGET)
	@$(CC) $(CFLAGS) -c -o $@ $<
$(OUT): $(OBJ) $(DRAW)
	@echo $@
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ $(OBJ) $(DRAW) $(LIBS)
