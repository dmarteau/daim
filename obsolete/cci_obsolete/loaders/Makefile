#-----------------------------------------------------
# Makefile for cci modules
#-----------------------------------------------------

DEPTH=../..

include $(DEPTH)/config/config.mk

DAIM_MODULES= CustomLoader \
                 $(NULL)

ifeq ($(DM_USE_BMP),1)
DAIM_MODULES += BMPLoader
endif

ifeq ($(DM_USE_PNG),1)
DAIM_MODULES += PNGLoader
endif

ifeq ($(DM_USE_TIFF),1)
DAIM_MODULES += TIFFLoader
endif

ifeq ($(DM_USE_JPEG),1)
DAIM_MODULES += JPEGLoader
endif

ifeq ($(DM_USE_GIF),1)
DAIM_MODULES += GIFLoader
endif

.PHONY: all_modules $(DAIM_MODULES)

all_modules: $(DAIM_MODULES)

$(DAIM_MODULES):
	@$(MAKE) -C $@

clean:
	@for d in $(DAIM_MODULES); do \
		$(MAKE) -C $$d clean; \
	done



