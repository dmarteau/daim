#

DEPTH= ../../../..

include $(DEPTH)/config/config.mk

MODULE_NAME=daim
LIBRARY_NAME=flatfld

MODULE_FILES=cciFlatfield.cpp \
             $(NULL)

IS_COMPONENT=1

IDLSRC = cciIFlatfield.idl \
         $(NULL)

MODULE_LIBS=-ldaimglue

DAIM_LIBKERNEL:=1

include $(topsrcdir)/config/rules.mk
