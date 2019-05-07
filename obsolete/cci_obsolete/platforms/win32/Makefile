#-----------------------------------------------------
# Makefile for w32 modules
#-----------------------------------------------------

DEPTH=../../..

include $(DEPTH)/config/config.mk

DAIM_MODULES  = 

# Frame grabbers
DAIM_MODULES += Framegrabbers/Twain


.PHONY: all_modules $(DAIM_MODULES)

all_modules: $(DAIM_MODULES)

$(DAIM_MODULES):
	@$(MAKE) -C $@

clean:
	@for d in $(DAIM_MODULES); do \
		$(MAKE) -C $$d clean; \
	done


