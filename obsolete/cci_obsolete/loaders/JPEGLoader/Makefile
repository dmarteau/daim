#
# Makefile for jpegldr.cci
#

DEPTH=../../..

include $(DEPTH)/config/config.mk

MODULE_NAME=jpegldr
MODULE_EXT=cci
MODULE_GROUP=loaders/
MODULE_INCL=
MODULE_FILES=CCI_JPEGLoader.cpp

ifeq ($(DM_USE_STATIC_LIBJPEG),1)
MODULE_LIBS=-ljpeg_s
else
MODULE_LIBS=-ljpeg
endif

DAIM_UTILITIES:=1
PURE_DAIM:=1

include $(DEPTH)/config/rules.mk
