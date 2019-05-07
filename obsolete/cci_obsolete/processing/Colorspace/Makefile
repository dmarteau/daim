#
# Makefile for colorspace.cci
#

DEPTH=../../..

include $(DEPTH)/config/config.mk

MODULE_NAME=colorspace
MODULE_EXT=cci
MODULE_GROUP=processing/
MODULE_FILES=CCI_Colorspace.cpp

DAIM_UTILITIES:=1
DAIM_LIBKERNEL:=1
DAIM_KERNEL:=1

COLORSPACES = GSL GSC HLS HSV OCP RGB YIQ YUV
CLEAN_EXTRA = clean_colorspaces

# Build this module then build color space modules colorspaces modules
colorspaces:  module
	@for d in $(COLORSPACES); do \
		$(MAKE) -C Colorspaces COLORSPACE=$$d; \
	done

clean_colorspaces:
	@for d in $(COLORSPACES); do \
		$(MAKE) -C Colorspaces COLORSPACE=$$d clean; \
	done

include $(DEPTH)/config/rules.mk




