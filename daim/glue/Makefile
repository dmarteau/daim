#

DEPTH= ../..
VPATH= ../src/

include $(DEPTH)/config/config.mk

MODULE_NAME=daim
LIBRARY_NAME=daimglue
MODULE_EXT=lib
MODULE_DEFS=DAIM_GLUE
MODULE_INCL=$(DEPTH)/daim_utilities/include

REQUIRES=cci

MODULE_FILES=cciDaimGlue.cpp \
             cciImageUtils.cpp \
             $(NULL)

EXPORTS = cciDaimGlue.h

include $(topsrcdir)/config/rules.mk
