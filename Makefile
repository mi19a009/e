# Make
.PHONY: all clean install uninst
all:
	@cd text   && $(MAKE)
	@cd viewer && $(MAKE)
clean:
	@cd text   && $(MAKE) $@
	@cd viewer && $(MAKE) $@
install:
	@cd text   && $(MAKE) $@
	@cd viewer && $(MAKE) $@
uninst:
	@cd text   && $(MAKE) $@
	@cd viewer && $(MAKE) $@
