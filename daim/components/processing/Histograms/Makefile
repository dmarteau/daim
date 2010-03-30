#

DEPTH= ../../../..

include $(DEPTH)/config/config.mk

MODULE_NAME=daim
LIBRARY_NAME=histograms

MODULE_FILES=cciHistograms.cpp \
             $(NULL)

IS_COMPONENT=1

IDLSRC = cciIHistograms.idl \
         $(NULL)

MODULE_LIBS=-ldaimglue

DAIM_LIBKERNEL:=1

include $(topsrcdir)/config/rules.mk
