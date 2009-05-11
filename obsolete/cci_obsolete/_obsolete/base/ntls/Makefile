#
# Makefile for ntls (no tools) module
#

DEPTH=../../..
VPATH = ../ui:$(DEPTH)/daim:$(DEPTH)/lib

include $(DEPTH)/config/config.mk

MODULE_NAME=ntls
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
             daim_glue.cpp \
             $(NULL)

MODULE_DEFS = NO_DAIM_USER_INTERFACE
MODULE_INCL = $(DEPTH)/daim

DAIM_KERNEL:=1
DAIM_LIBKERNEL:=1
DAIM_UTILITIES:=1

include $(DEPTH)/config/rules.mk

