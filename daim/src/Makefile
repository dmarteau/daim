#
# Makefile for daim dll
#

DEPTH= ../..
VPATH = ./:../components/processing/ColorSpaces/:../components/loaders/

include $(DEPTH)/config/config.mk

MODULE_NAME  = daim
LIBRARY_NAME = daim
MODULE_EXT   = dll
MODULE_DEFS  = DAIM_BUILD CCI_TRANSLATE_MODULE_ENTRY_POINT 

MODULE_FILES=daim.cpp \
             cciImageUtils.cpp \
             cciImageList.cpp \
             cciFilterContext.cpp \
             cciMatrix.cpp \
             cciDaimService.cpp \
             cciImageShell.cpp \
             cciDaimModule.cpp \
             cciScriptableRegion.cpp \
             cciLoaderService.cpp \
             cciScriptableImage.cpp \
             cciImageListImage.cpp \
             cciStorageImage.cpp \
             RGB.cpp \
             $(NULL)

MODULE_LIBS = -lcci_s

EXPORTS=cciImageUtils.h \
        cciDaimModule.h \
        cciImageContainerUtils.h \
        $(NULL)

REQUIRES=cci

DAIM_LIBKERNEL:=1
DAIM_MATRIX:=1

MODULE_LIBS += -ldaim_dmatrix

include $(topsrcdir)/config/rules.mk
