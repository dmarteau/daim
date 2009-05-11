#

DEPTH= ../../../..

include $(DEPTH)/config/config.mk

MODULE_NAME=daim
LIBRARY_NAME=ldrsrvc

MODULE_FILES=cciLoaderService.cpp \
             $(NULL)

DIRS=drivers

IS_COMPONENT=1

IDLSRC = cciIDriverProxy.idl   \
         cciISurfaceDriver.idl \
         cciIRemoteSurface.idl \
         cciILoaderService.idl \
         $(NULL)

DAIM_UTILITIES:=1

include $(DEPTH)/config/rules.mk
