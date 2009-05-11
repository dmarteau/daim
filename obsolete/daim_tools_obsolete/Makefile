#
# Makefile for daim_tools dll
#

DEPTH= ..
VPATH=./source

include $(DEPTH)/config/config.mk

MODULE_NAME = daim_tools
MODULE_EXT  = dll
MODULE_INCL = ./include
MODULE_DEFS = DAIM_TOOLS_BUILD

MODULE_FILES=dmStyleManager.cpp \
             dmToolArrowPrefs.cpp \
             dmToolImagePrefs.cpp \
             dmToolPrefs.cpp \
             dmToolPrefsVisitor.cpp \
             dmToolTextPrefs.cpp \
             dmToolClasses.cpp \
             dmToolFactory.cpp \
             dmAreaEdit.cpp \
             dmArrows.cpp \
             dmCarrets.cpp \
             dmEventHandler.cpp \
             dmEventNotifier.cpp \
             dmGraphics.cpp \
             dmGroup.cpp \
             dmImageFrame.cpp \
             dmLineExt.cpp \
             dmRubber.cpp \
             dmShapeVisitor.cpp \
             dmText.cpp \
             dmToolGroup.cpp \
             dmToolKit.cpp \
             dmToolManager.cpp \
             dmTools.cpp \
             dmToolVisitor.cpp

DAIM_KERNEL:=1
DAIM_UTILITIES:=1

include $(DEPTH)/config/rules.mk
