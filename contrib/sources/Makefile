#-----------------------------------------------------
# Makefile for contrib modules
#-----------------------------------------------------

DEPTH=../..

include $(DEPTH)/config/config.mk

CONTRIB_MODULES=

ifeq ($(BUILD_TYPE),-win32-msvc)
  CONTRIB_MODULES += specfun 
endif

ifeq ($(DM_USE_STATIC_LIBJPEG),1)
  DM_USE_STATIC_ZLIB:=1
  CONTRIB_MODULES += libjpeg
endif

ifeq ($(DM_USE_STATIC_LIBPNG),1)
  DM_USE_STATIC_ZLIB:=1
  CONTRIB_MODULES += libpng
endif

ifeq ($(DM_USE_STATIC_ZLIB),1)
  CONTRIB_MODULES += zlib
endif

ifeq ($(DM_USE_STATIC_LIBGIF),1)
  CONTRIB_MODULES += libungif
endif


.PHONY: all_modules $(CONTRIB_MODULES)

all_modules: $(CONTRIB_MODULES)

$(CONTRIB_MODULES):
	@$(MAKE) -C $@

clean:
	@for d in $(CONTRIB_MODULES); do \
		$(MAKE) -C $$d clean; \
	done


