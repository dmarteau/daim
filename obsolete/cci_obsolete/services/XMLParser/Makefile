#-----------------------------------------------------
# Makefile for xmlparser
#-----------------------------------------------------

DEPTH=../../..

include $(DEPTH)/config/config.mk

MODULE_NAME=xmlparser
MODULE_EXT=cci
MODULE_FILES=CCI_XMLParser.cpp \
             utf82latin1.cpp

# Use static libs 
ifeq ($(OS_ARCH),WINNT)
 MODULE_LIBDIR=$(DAIM_EXTRA_LIBRARY_PATH)/lib
 MODULE_INCL="$(DAIM_EXTRA_LIBRARY_PATH)/includes"
endif

ifdef DM_LIBEXPAT
 MODULE_LIBS=$(DM_LIBEXPAT)
else
 MODULE_LIBS=-lexpat
endif


DAIM_UTILITIES:=1

include $(DEPTH)/config/rules.mk
