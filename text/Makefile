# Make Text
DOMAIN  := LC_MESSAGES/github-mi19a009-e.mo
INSTALL := ~/.local/share/locale
LOCALE  := ../locale
LINGUAS := ja
MESSAGES = $(addprefix $(LOCALE)/, $(addsuffix /$(DOMAIN), $(LINGUAS)))
all: $(MESSAGES)
clean:
	$(RM) $(wildcard $(LOCALE)/*/$(DOMAIN))
install:
	@$(MAKE) LOCALE=$(INSTALL)
uninst:
	@$(MAKE) clean LOCALE=$(INSTALL)
$(MESSAGES): $(LOCALE)/%/$(DOMAIN): %.pot
	@echo $@
	@mkdir -p $(dir $@)
	@msgfmt -o $@ $<
