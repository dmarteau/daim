#
# Makefile for gifldr.cci
#

DEPTH=../../..

include $(DEPTH)/config/config.mk

MODULE_NAME=gifldr
MODULE_EXT=cci
MODULE_GROUP=loaders/
MODULE_FILES=CCI_GIFLoader.cpp

ifeq ($(DM_USE_STATIC_LIBGIF),1)
MODULE_LIBS=-lungif_s
else
MODULE_LIBS=-lungif
endif

DAIM_UTILITIES:=1
PURE_DAIM:=1

include $(DEPTH)/config/rules.mk

