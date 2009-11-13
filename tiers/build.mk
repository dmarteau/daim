DEPTH = ..

include $(DEPTH)/config/config.mk

.PHONY: all

ifdef DAIM_GDAL
TIERS += gdal
endif

ifdef TIERS
all: $(TIERS)
	$(MAKE) -C $^ -f setup.mk
endif
