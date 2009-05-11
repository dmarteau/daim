#
# Makefile template for colorspaces
#
DEPTH=../../../..

include $(DEPTH)/config/config.mk

MODULE_NAME=daim
LIBRARY_NAME=$(COLORSPACE)

MODULE_FILES=$(COLORSPACE).cpp

IS_COMPONENT=1

DAIM_LIBKERNEL:=1

include $(DEPTH)/config/rules.mk
