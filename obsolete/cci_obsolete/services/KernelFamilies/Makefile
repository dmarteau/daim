#
# Makefile for kernelfamilies.cci
#

DEPTH=../../..
VPATH=$(DEPTH)/daim_kernel/source/templates/processing

include $(DEPTH)/config/config.mk

MODULE_NAME=kernelfamilies
MODULE_EXT=cci
MODULE_GROUP=processing/
MODULE_INCL=$(DEPTH)/daim_kernel/include
MODULE_FILES=CCI_KernelFamilies.cpp \
             dmKernelsMngr.cpp \
             dmKernelsLoader.cpp \
             dmKernelsSet.cpp \
             dmKernelFamily.cpp


DAIM_UTILITIES:=1
PURE_DAIM:=1

include $(DEPTH)/config/rules.mk
