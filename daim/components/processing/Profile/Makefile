#

DEPTH= ../../../..
VPATH=grsz/

include $(DEPTH)/config/config.mk

MODULE_NAME=daim
LIBRARY_NAME=profile
MODULE_INCL=$(DEPTH)/daim/src

MODULE_FILES= grszcore.cpp \
              cciProfile.cpp \
              $(NULL)

IS_COMPONENT=1

IDLSRC = cciIProfile.idl \
         cciIIntercepts.idl \
         $(NULL)

DAIM_LIBKERNEL:=1
DAIM_MATRIX:=1

include $(topsrcdir)/config/rules.mk
