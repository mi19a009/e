# Make Text
DOMAIN  := LC_MESSAGES/github-mi19a009-e.mo
LOCALE  := ../locale
INSTALL := LOCALE=$(HOME)/.local/share/locale
LINGUAS := ja
MESSAGES = $(addprefix $(LOCALE)/, $(addsuffix /$(DOMAIN), $(LINGUAS)))
all: $(MESSAGES)
clean:
	$(RM) $(wildcard $(LOCALE)/*/$(DOMAIN))
install:
	@$(MAKE) $(INSTALL)
uninst:
	@$(MAKE) clean $(INSTALL)
$(MESSAGES): $(LOCALE)/%/$(DOMAIN): %.pot
	@echo $@
	@mkdir -p $(dir $@)
	@msgfmt -o $@ $<
