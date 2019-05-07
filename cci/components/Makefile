#
# Makefile for cci glue
#

DEPTH= ../..
VPATH = ./:../glue:../io

include $(DEPTH)/config/config.mk

#
# cci static lib (this is cci implementation, NOT glue)
#

MODULE_NAME  = cci
LIBRARY_NAME = cci_s
MODULE_EXT   = lib
MODULE_INCL  = ..
MODULE_DEFS  = DAIM_BUILD CCI_TRANSLATE_MODULE_ENTRY_POINT

DAIM_INTERNAL := 1

MODULE_FILES=cciComponentManagerUtils.cpp \
             cciInterfaceRequestorUtils.cpp \
             cciGenericFactoryUtils.cpp \
             cciSupportsImpl.cpp \
             cciCOMPtr.cpp \
             cciEnumeratorUtils.cpp \
             cciAllocatorImpl.cpp \
             cciCOMInit.cpp \
             cciComponentManager.cpp \
             cciGenericFactory.cpp \
             cciMemoryImpl.cpp \
             cciString.cpp \
             cciNativeModuleLoader.cpp \
             cciStaticModuleLoader.cpp \
             cciId.cpp \
             cciMemoryInputStream.cpp \
             cciFileStreams.cpp  \
             cciStreamModule.cpp \
             cciCategoryManager.cpp \
             $(NULL)

DAIM_UTILITIES:=1

include $(topsrcdir)/config/rules.mk
