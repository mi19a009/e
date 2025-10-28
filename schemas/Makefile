# Make Schemas
SCHEMA := \
	draw.gschema.xml \
	viewer.gschema.xml
.PHONY: all clean
all: gschemas.compiled
clean:
	$(RM) gschemas.compiled gschemas.valid
gschemas.compiled: gschemas.valid
	@echo $@
	@glib-compile-schemas .
gschemas.valid: $(SCHEMA)
	@echo $@
	@glib-compile-schemas --strict --dry-run .
	@touch $@
