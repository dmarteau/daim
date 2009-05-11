#
# Makefile for pens.cci
#

DEPTH=../../..
VPATH=./pens:$(DEPTH)/daim_tools/source/advtools:$(DEPTH)/daim_kernel/source/common

include $(DEPTH)/config/config.mk

# In debug mode, do not build extension package
ifneq ($(BUILD_DEBUG),1)
  MODULE_PACKAGE=Extensions
endif

MODULE_NAME=pens
MODULE_EXT=cci
MODULE_GROUP=drawing/
MODULE_FILES=CCI_Pens.cpp

DAIM_UTILITIES:=1
DAIM_LIBKERNEL:=1
DAIM_KERNEL:=1
DAIM_TOOLS:=1

include $(DEPTH)/config/rules.mk
