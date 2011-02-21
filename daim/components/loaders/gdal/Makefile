#

DEPTH= ../../../..

include $(DEPTH)/config/config.mk

MODULE_NAME=loaders
ifdef DM_GDAL_DLL
LIBRARY_NAME=gdaldrv
else
LIBRARY_NAME=gdalmindrv

GDAL_HOME=$(BUILD_DIR)/gdal
GDAL_INCLUDE=$(GDAL_HOME)/include
GDAL_LIB=$(GDAL_HOME)/lib

endif

MODULE_INCL=$(GDAL_INCLUDE)

MODULE_FILES=gdaldrv.cpp \
             gdalsurface.cpp \
             gdalproxy.cpp \
             gdalColorTable.cpp \
             $(NULL)

IS_COMPONENT=1

ifeq ($(OS_ARCH),WINNT)

MODULE_LIBDIR=$(GDAL_LIB)

ifdef DM_GDAL_DLL
MODULE_LIBS=-l$(GDAL_PREFIX)gdal_i
else
MODULE_DEFS=GDAL_MINIMAL
MODULE_LIBS=-l$(GDAL_PREFIX)gdal
endif

else

ifdef DM_GDAL_DLL
MODULE_LIBDIR=$(GDAL_LIB)
MODULE_LIBS=-l$(GDAL_PREFIX)gdal
else
MODULE_DEFS = GDAL_MINIMAL
MODULE_STATIC_LIBS=$(GDAL_LIB)/lib$(GDAL_PREFIX)gdal.a
endif

endif

REQUIRES=daim

DAIM_KERNEL:=1

include $(topsrcdir)/config/rules.mk
