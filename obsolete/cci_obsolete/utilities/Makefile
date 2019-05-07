#-----------------------------------------------------
# Makefile for utilities
#-----------------------------------------------------

DEPTH=../..

include $(DEPTH)/config/config.mk

MODULES= \
  pngdecode \
  pngencode \
  $(NULL)

.PHONY: all_modules $(MODULES)

all_modules: $(MODULES)

$(MODULES):
	@$(MAKE) -C $@

clean:
	@for d in $(MODULES); do \
		$(MAKE) -C $$d clean; \
	done
