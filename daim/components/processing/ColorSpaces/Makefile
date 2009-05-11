#
# Makefile for colorspaces
#
DEPTH=../../../..

include $(DEPTH)/config/config.mk

MODULE_NAME=daim

COLORSPACES = GSL GSC HLS HSV OCP YIQ YUV HSI

libs::
	@for d in $(COLORSPACES); do \
		$(MAKE) -f colorspace.mk COLORSPACE=$$d; \
	done

clean::
	@for d in $(COLORSPACES); do \
		$(MAKE) -f colorspace.mk COLORSPACE=$$d clean; \
	done

EXPORTS = cciColorSpaces.h

include $(DEPTH)/config/rules.mk
