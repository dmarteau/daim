#-----------------------------------------------------
# Makefile for cci modules
#-----------------------------------------------------

DEPTH=..

include $(DEPTH)/config/config.mk

DAIM_MODULES= \
  processing/BinaryMath \
  processing/Filters \
  processing/ImageMath \
  processing/ZFocus \
  processing/Histograms \
  processing/ImageShift \
  processing/Edges \
  processing/Colorspace \
  processing/Fractals \
  processing/Transforms \
  processing/ImageMap \
  processing/ImageParts \
  processing/Pyramid \
  processing/Morphology \
  processing/Profile \
  processing/LeastSquare \
  processing/ImageStat \
  processing/FlatField \
  processing/Draw2d \
  services/XMLParser \
  services/KernelFamilies \
  services/PropertyList \
  services/StorageMap \
  $(NULL)


ifeq ($(OS_ARCH),WINNT)
DAIM_MODULES += platforms/win32
endif

.PHONY: all_modules $(DAIM_MODULES)

all_modules: $(DAIM_MODULES)

$(DAIM_MODULES):
	@$(MAKE) -C $@

clean:
	@for d in $(DAIM_MODULES); do \
		$(MAKE) -C $$d clean; \
	done


