# Make Draw
NAME     := com.github.mi19a009.Draw
ENTRY    := $(ENTRIES)/$(NAME).desktop
EXEC     := $(BIN)/draw
ICON     := $(PWD)/icons/48x48/actions/drawing.png
OBJ      := $(TARGET)/drawing.gresources.o
SCHEMA   := $(SCHEMAS)/$(NAME).gschema.xml
ASSETS   := \
	$(wildcard *.ui) \
	$(wildcard gtk/*.ui) \
	$(wildcard icons/48x48/actions/*.png)
DRAW     := \
	$(TARGET)/drawing.o \
	$(TARGET)/drawingapplication.o \
	$(TARGET)/drawingapplicationwindow.o
override CFLAGS += -DGETTEXT_PATH=\"$(LOCALE)\"
.PHONY: all clean install uninst
all: $(EXEC) $(SCHEMA)
install: $(EXEC) $(SCHEMA) $(ENTRY)
clean:
	$(CLEAN) $(SCHEMA)
uninst:
	$(RM) $(SCHEMA) $(ENTRY)
# Sources
drawing.gresources.c: drawing.gresources.xml $(ASSETS)
	@echo $@
	@glib-compile-resources $< --target $@ --generate-source
# Objects
$(OBJ): $(TARGET)/%.o: %.c
	@echo $@
	@mkdir -p $(TARGET)
	@$(CC) $(CFLAGS) -c -o $@ $<
$(DRAW): $(TARGET)/%.o: %.c drawing.h
	@echo $@
	@mkdir -p $(TARGET)
	@$(CC) $(CFLAGS) -c -o $@ $<
$(EXEC): $(OBJ) $(DRAW)
	@echo $@
	@mkdir -p $(BIN)
	@$(CC) $(CFLAGS) -o $@ $(OBJ) $(DRAW) $(LIBS)
# Desktop Entries
$(ENTRY): drawing.desktop $(ICON)
	@echo $@
	@mkdir -p $(ENTRIES)
	@sed -e "s#Exec=#Exec=$(EXEC)#" -e "s#Icon=#Icon=$(ICON)#" $< > $@
# Schemas
drawing.gschema.valid: drawing.gschema.xml
	@echo $@
	@glib-compile-schemas --strict --dry-run --schema-file=$<
	@touch $@
$(SCHEMA): drawing.gschema.xml drawing.gschema.valid
	@echo $@
	@mkdir -p $(SCHEMAS)
	@cp $< $@
