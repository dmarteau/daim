#
# Makefile for wingraphics
#

DEPTH=../../../..

include $(DEPTH)/config/config.mk

MODULE_NAME=wingraphics
MODULE_EXT=cci
MODULE_GROUP=win32/
MODULE_LIBS =-lgdi32
MODULE_FILES=CCI_WinGraphics.cpp \
             dmWinGraphics.cpp \
             dmWinNativeImage.cpp 
             
MODULE_RES=dmResources.rc

DAIM_UTILITIES:=1
DAIM_KERNEL:=1
DAIM_TOOLS:=1
PURE_DAIM:=1

include $(DEPTH)/config/rules.mk
