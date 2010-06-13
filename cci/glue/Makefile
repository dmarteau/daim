#
# Makefile for cci glue
#

DEPTH= ../..
VPATH = ./

include $(DEPTH)/config/config.mk

MODULE_NAME  = cci
LIBRARY_NAME = cciglue
MODULE_EXT   = lib
MODULE_DEFS  = DAIM_GLUE

DIRS=static

MODULE_FILES= cciComponentManagerUtils.cpp \
              cciInterfaceRequestorUtils.cpp \
              cciGenericFactoryUtils.cpp \
              cciSupportsImpl.cpp \
              cciCOMPtr.cpp \
              cciEnumeratorUtils.cpp \
              cciWeakReference.cpp \
              cciStubs.cpp \
              cciCOMGlue.cpp \
              cciId.cpp \
              $(NULL)

EXPORTS= cciCore.h \
         cciErrors.h \
         cciId.h  \
         cciDebug.h \
         cciStubs.h \
         cciISupportsBase.h \
         cciSupportsImpl.h \
         cciSupportsUtils.h \
         cciIGenericFactory.h \
         cciComponentManagerUtils.h \
         cciServiceManagerUtils.h \
         cciInterfaceRequestorUtils.h \
         cciCOMPtr.h \
         cciMemory.h \
         cciPrivateHook.h \
         cciString.h \
         cciIAllocator.h \
         cciNativeWrapper.h \
         cciIWeakReferenceUtils.h \
         cciWeakReference.h \
         $(NULL)

include $(topsrcdir)/config/rules.mk
