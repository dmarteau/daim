#
# Makefile for daim_libkernel lib
#


DEPTH= ..
SRCDIR= $(DEPTH)/daim_kernel/source
VPATH = $(SRCDIR):$(SRCDIR)/images:$(SRCDIR)/regions:$(SRCDIR)/common:\
        $(SRCDIR)/templates:$(SRCDIR)/templates/processing:$(SRCDIR)/documents:\
        $(SRCDIR)/operators:$(SRCDIR)/tools

include $(DEPTH)/config/config.mk

MODULE_NAME=daim_libkernel
LIBRARY_NAME=daim_libkernel
MODULE_EXT =lib
MODULE_DEFS=DAIM_KERNEL
MODULE_INCL=$(DEPTH)/daim_kernel/include

DAIM_INTERNAL:=1

MODULE_FILES=dmUserLib.cpp \
             dmBinaryMorpho_1.cpp \
             dmBinaryMorpho_2.cpp \
             dmColorHistogram.cpp \
             dmConvKernelsImpl.cpp \
             dmDensityMapImpl.cpp \
             dmFTFilters.cpp \
             dmHistogram.cpp \
             dmHistogramImpl.cpp \
             dmKernelFamily.cpp \
             dmNagoFilterImpl.cpp \
             dmObjectDetection.cpp \
             dmPartition.cpp \
             dmPartitionImpl.cpp \
             dmImageFTImpl.cpp \
             dmMetrics.cpp \
             dmColorMaps.cpp \
             dmCooccurrences.cpp \
             dmIntensityProfile.cpp \
             dmRegionTree.cpp \
             dmRegionsProcessing.cpp \
             dmAccumulator.cpp \
             dmAddMulImage.cpp \
             dmArithmetics.cpp \
             dmCannyFilter.cpp \
             dmCompareAndCopy.cpp \
             dmCompareAndCopyMask.cpp \
             dmConvolution.cpp \
             dmCopyIfMaskCondition.cpp \
             dmCreateRegions.cpp \
             dmDensityMap.cpp \
             dmDownSampling.cpp \
             dmExtendMap.cpp \
             dmFillRGB.cpp \
             dmFillScalar.cpp \
             dmFlipCopy.cpp \
             dmImageFT.cpp \
             dmImageMatrix.cpp \
             dmInvertMap.cpp \
             dmLinearFilters.cpp \
             dmLocalMax.cpp \
             dmMakePeriodic.cpp \
             dmCreateMotif.cpp \
             dmMorphology.cpp \
             dmNagaoFilter.cpp \
             dmRotateCopy.cpp \
             dmScaleTo8Bits.cpp \
             dmThreshold.cpp \
             dmTransformMap.cpp \
             dmUpSampling.cpp \
             dmGammaCorrection.cpp \
             dmNonMaxSuppress.cpp \
             dmBlendImage.cpp \
             dmRGBThreshold.cpp \
             dmEnhanceContrast.cpp \
             dmRotateImage.cpp \
             dmLinearStretch.cpp \
             dmStretchImage.cpp \
             dmRotation.cpp \
             dmEdgeSpreadFunction.cpp \
             dmImageStats.cpp \
             dmScaleImage.cpp


DAIM_UTILITIES:=1
DAIM_MATRIX:=1

include $(topsrcdir)/config/rules.mk
