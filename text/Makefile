# Make Text
DOMAIN   := LC_MESSAGES/text.mo
LINGUAS  := ja
MESSAGES := $(addprefix $(LOCALE)/, $(addsuffix /$(DOMAIN), $(LINGUAS)))
all: $(MESSAGES)
$(MESSAGES): $(LOCALE)/%/$(DOMAIN): %.pot
	@echo $@
	@mkdir -p $(dir $@)
	@msgfmt -o $@ $<
