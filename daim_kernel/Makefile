#
# Makefile for daim_kernel dll
#

DEPTH= ..
SRCDIR=./source

VPATH = $(SRCDIR):$(SRCDIR)/images:$(SRCDIR)/regions:$(SRCDIR)/common:\
        $(SRCDIR)/templates:$(SRCDIR)/templates/processing:$(SRCDIR)/documents:\
        $(SRCDIR)/operators

include $(DEPTH)/config/config.mk

MODULE_NAME  = daim
LIBRARY_NAME = daim_kernel
MODULE_EXT   = dll
MODULE_INCL  = ./include
MODULE_DEFS  = DAIM_KERNEL_BUILD

DIRS=include

MODULE_FILES= \
             dmImage.cpp \
             dmImageBuffer.cpp \
             dmImageConversions.cpp \
             dmImageDescriptor.cpp \
             dmImageMath.cpp \
             dmImageMathImpl.cpp \
             dmCoordinates.cpp \
             dmRegionLineBuffer.cpp \
             dmRegionLines.cpp \
             dmRegions.cpp \
             dmMetrics.cpp \
             dmSpatialUnits.cpp \
             dmUserLib.cpp \
             dmColorHistogram.cpp \
             dmConvKernelsImpl.cpp \
             dmDensityMapImpl.cpp \
             dmHistogram.cpp \
             dmHistogramImpl.cpp \
             dmKernelFamily.cpp \
             dmImageBase.cpp \
             dmRGBTraits.cpp \
             dmRoiOperations.cpp \
             dmAddMulImage.cpp \
             dmArithmetics.cpp \
             dmFillScalar.cpp \
             dmMakePeriodic.cpp \
             dmScaleTo8Bits.cpp \
             dmKernelBase.cpp \
             dmLut.cpp

DAIM_UTILITIES:=1
DAIM_MATRIX:=1

include $(topsrcdir)/config/rules.mk


