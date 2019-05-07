#
# Makefile for advtools.cci
#

DEPTH=../../..
VPATH=$(DEPTH)/daim_tools/source/advtools:$(DEPTH)/daim_kernel/source/common


include $(DEPTH)/config/config.mk

MODULE_NAME=advtools
MODULE_EXT=cci
MODULE_FILES=CCI_AdvTools.cpp \
             dmCaliper.cpp \
             dmCircleEdit.cpp \
             dmDrawUtils.cpp \
             dmPolyEditEx.cpp \
             dmPtsCircleEdit.cpp \
             dmPtsCircleShape.cpp \
             dmRectEditEx.cpp \
             dmRectShapeEx.cpp \
             dmSector.cpp \
             dmToolClassesEx.cpp \
             dmMetrics.cpp
             
DAIM_UTILITIES:=1
DAIM_LIBKERNEL:=1
DAIM_KERNEL:=1
DAIM_TOOLS:=1

include $(DEPTH)/config/rules.mk
