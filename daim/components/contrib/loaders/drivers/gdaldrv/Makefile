#

DEPTH= ../../../../../..

include $(DEPTH)/config/config.mk

MODULE_NAME=contrib
ifdef DM_GDAL_DLL
LIBRARY_NAME=gdaldrv
else
LIBRARY_NAME=gdalmindrv
endif

# If GDAL_HOME si not set 
# build our own version of gdal
ifeq ($(GDAL_INTERNAL),1)

GDAL_HOME=$(DIST_REPOSITORY)/gdal
GDAL_INCLUDE=$(GDAL_HOME)/include
GDAL_LIB=$(GDAL_HOME)/lib

DIRS+=gdal-build

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
MODULE_LIBS=-lgdal_i
else
MODULE_DEFS=GDAL_MINIMAL
MODULE_LIBS=-lgdal
endif

else

ifdef DM_GDAL_DLL
MODULE_LIBDIR=$(GDAL_LIB)
MODULE_LIBS=-lgdal
else
MODULE_DEFS = GDAL_MINIMAL
MODULE_STATIC_LIBS=$(GDAL_LIB)/libgdal.a
endif

endif

REQUIRES=daim

DAIM_KERNEL:=1

include $(DEPTH)/config/rules.mk
