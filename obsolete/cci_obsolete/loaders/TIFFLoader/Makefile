#
# Makefile for tiffldr.cci
#

DEPTH=../../..

include $(DEPTH)/config/config.mk

MODULE_NAME=tiffldr
MODULE_EXT=cci
MODULE_GROUP=loaders/
MODULE_FILES=CCI_TIFFLoader.cpp


# Use static libs 
ifeq ($(OS_ARCH),WINNT)
 MODULE_LIBDIR=$(DAIM_EXTRA_LIBRARY_PATH)/lib
 MODULE_INCL="$(DAIM_EXTRA_LIBRARY_PATH)/includes"
endif

MODULE_LIBS=-ltiff

ifeq ($(BUILD_TYPE),-win32-msvc)
 MODULE_LIBS += -ljpeg
endif

DAIM_UTILITIES:=1
PURE_DAIM:=1

include $(DEPTH)/config/rules.mk

