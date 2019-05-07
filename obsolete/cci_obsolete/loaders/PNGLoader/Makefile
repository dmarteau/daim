#
# Makefile for pngldr.cci
#

DEPTH=../../..

include $(DEPTH)/config/config.mk

MODULE_NAME=pngldr
MODULE_EXT=cci
MODULE_GROUP=loaders/
MODULE_FILES=CCI_PNGLoader.cpp

ifeq ($(DM_USE_STATIC_LIBPNG),1)
MODULE_LIBS=-lpng_s -lz_s
else
MODULE_LIBS=-lpng -lz
endif

DAIM_UTILITIES:=1
PURE_DAIM:=1

include $(DEPTH)/config/rules.mk

