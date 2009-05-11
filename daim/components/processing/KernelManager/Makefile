#
# Makefile for kernelfamilies.cci
#
DEPTH=../../../..
VPATH=$(DEPTH)/daim_kernel/source/templates/processing

include $(DEPTH)/config/config.mk

MODULE_NAME=daim
LIBRARY_NAME=kernelmngr
MODULE_INCL=$(DEPTH)/daim_kernel/include

MODULE_FILES=cciKernelManagerModule.cpp \
             cciScriptableKernelFamily.cpp \
             cciKernelService.cpp \
             dmKernelFamily.cpp \
             $(NULL)

IS_COMPONENT=1

DAIM_UTILITIES:=1

include $(DEPTH)/config/rules.mk
