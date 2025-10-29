# Make
BIN     := $(PWD)/build
CFLAGS  := $(shell pkg-config gtk4 --cflags) -Wall -DGETTEXT_CODESET=\"UTF-8\" -DGETTEXT_PACKAGE=\"text\"
CLEAN   := rm -rf build debug locale release $(wildcard *.compiled) $(wildcard *.gresources.c) $(wildcard *.valid)
DEBUG   := "BIN := $(PWD)/debug" "TARGET := debug"
ENTRIES := $(HOME)/.local/share/applications
LIBS    := $(shell pkg-config gtk4 --libs)
LOCALE  := $(PWD)/locale
RELEASE := "BIN := $(PWD)/release" "TARGET := release"
SCHEMAS := $(HOME)/.local/share/glib-2.0/schemas
TARGET  := build
export BIN CFLAGS CLEAN ENTRIES LIBS LOCALE SCHEMAS TARGET
.PHONY: all clean debug draw install release schemas text uninst viewer
all: text viewer schemas
clean:
	$(CLEAN)
	@cd viewer && $(MAKE) clean
debug:
	@$(MAKE) $(DEBUG) \
	"CFLAGS := $(CFLAGS) -g -DG_ENABLE_DEBUG"
install: text
	@cd viewer && $(MAKE) install $(RELEASE)
	glib-compile-schemas $(SCHEMAS)
release:
	@$(MAKE) $(RELEASE) \
	"CFLAGS := $(CFLAGS) -O2 -DNDEBUG -DG_DISABLE_ASSERT -DG_DISABLE_CAST_CHECKS"
schemas:
	glib-compile-schemas $(SCHEMAS)
uninst:
	@cd viewer && $(MAKE) uninst $(RELEASE)
	glib-compile-schemas $(SCHEMAS)
draw text viewer:
	@cd $@ && $(MAKE)
