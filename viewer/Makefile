# Make Viewer
NAME     := com.github.mi19a009.PictureViewer
ENTRY    := $(ENTRIES)/$(NAME).desktop
EXEC     := $(BIN)/viewer
ICON     := $(PWD)/icons/48x48/actions/viewer.png
OBJ      := $(TARGET)/viewer.gresources.o
SCHEMA   := $(SCHEMAS)/$(NAME).gschema.xml
ASSETS   := \
	$(wildcard *.ui) \
	$(wildcard gtk/*.ui) \
	$(wildcard icons/48x48/actions/*.png)
VIEWER   := \
	$(TARGET)/viewer.o \
	$(TARGET)/viewerapplication.o \
	$(TARGET)/viewerapplicationwindow.o
override CFLAGS += -DGETTEXT_PATH=\"$(LOCALE)\"
.PHONY: all clean install uninst
all: $(EXEC) $(SCHEMA)
install: $(EXEC) $(SCHEMA) $(ENTRY)
clean:
	$(CLEAN) $(SCHEMA)
uninst:
	$(RM) $(SCHEMA) $(ENTRY)
# Sources
viewer.gresources.c: viewer.gresources.xml $(ASSETS)
	@echo $@
	@glib-compile-resources $< --target $@ --generate-source
# Objects
$(OBJ): $(TARGET)/%.o: %.c
	@echo $@
	@mkdir -p $(TARGET)
	@$(CC) $(CFLAGS) -c -o $@ $<
$(VIEWER): $(TARGET)/%.o: %.c viewer.h
	@echo $@
	@mkdir -p $(TARGET)
	@$(CC) $(CFLAGS) -c -o $@ $<
$(EXEC): $(OBJ) $(VIEWER)
	@echo $@
	@mkdir -p $(BIN)
	@$(CC) $(CFLAGS) -o $@ $(OBJ) $(VIEWER) $(LIBS)
# Desktop Entries
$(ENTRY): viewer.desktop $(ICON)
	@echo $@
	@mkdir -p $(ENTRIES)
	@sed -e "s#Exec=#Exec=$(EXEC)#" -e "s#Icon=#Icon=$(ICON)#" $< > $@
# Schemas
viewer.gschema.valid: viewer.gschema.xml
	@echo $@
	@glib-compile-schemas --strict --dry-run --schema-file=$<
	@touch $@
$(SCHEMA): viewer.gschema.xml viewer.gschema.valid
	@echo $@
	@mkdir -p $(SCHEMAS)
	@cp $< $@
