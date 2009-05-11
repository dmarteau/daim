#
# Makefile for imageinfo
#

DEPTH=../../..
VPATH = ./:$(DEPTH)/daim:$(DEPTH)/lib:$(DEPTH)/daim_tools/source

include $(DEPTH)/config/config.mk

MODULE_NAME=ui
MODULE_EXT=cci

MODULE_FILES= \
             CCI_UserInterface.cpp \
             CCI_Document.cpp \
             CCI_Info.cpp \
             daim_info.cpp \
             daim_info_doc.cpp \
             daim_cmdtarget.cpp \
             daim_unitsprefs.cpp \
             daim_msg.cpp \
             daim_glue.cpp

MODULE_FILES+= \
             CCI_Graphics.cpp \
             CCI_Manager.cpp \
             CCI_ToolInfo.cpp \
             CCI_EventListener.cpp \
             daim_info_cmds.cpp \
             daim_info_mngr.cpp \
             daim_info_tools.cpp \
             daim_manager.cpp \
             daim_masks_utils.cpp \
             daim_roi.cpp \
             daim_layers.cpp \
             dmRegionEdit.cpp \
             dmRegionEditProto.cpp \
             $(NULL)

MODULE_INCL = $(DEPTH)/daim

DAIM_TOOLS:=1
DAIM_KERNEL:=1
DAIM_LIBKERNEL:=1
DAIM_UTILITIES:=1

include $(DEPTH)/config/rules.mk

