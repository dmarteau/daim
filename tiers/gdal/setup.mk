DEPTH = ../..

include $(DEPTH)/config/config.mk

.PHONY: co apply_patches

ifndef GDAL_CHECKOUT
GDAL_CHECKOUT=svn export https://svn.osgeo.org/gdal/tags/1.5.4/gdal
endif

GDAL_SRC=$(DEPTH)/../gdal

_curdir := $(shell pwd)

PATCH_FILES= \
     p1.patch \
     p2.patch \
     p3.patch \
     $(NULL)

all: co

co:
	@echo "Checking GDAL configuration"
	@if test ! -d $(GDAL_SRC); then \
	    echo "Checkout GDAL source from $(GDAL_CHECKOUT)"; \
		cd $(DEPTH)/..;$(GDAL_CHECKOUT) gdal; \
		cd gdal;$(foreach p,$(PATCH_FILES),patch -p0 < "$(_curdir)/patches/$(p)";) echo; \
	fi

apply_patches:
	cd $(DEPTH)/../gdal;$(foreach p,$(PATCH_FILES),patch -p0 < "$(_curdir)/patches/$(p)";) echo
