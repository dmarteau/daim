DEPTH = ..

include $(DEPTH)/config/config.mk

.PHONY: all

ifeq ($(DAIM_GDAL),1)
TIERS += gdal
endif

ifdef TIERS
all: $(TIERS)
	$(MAKE) -C $^ -f setup.mk
endif
