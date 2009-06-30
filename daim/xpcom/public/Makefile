#
# Makefile for daim dll
#

DEPTH= ../../..

include $(DEPTH)/config/config.mk

MODULE_NAME = xpcom

XPCOM_DIST=$(DIST_REPOSITORY)/xpcom

XPCOM_EXPORTS=$(DEPTH)/cci/xpcom/cciIXPCOMSupports.idl \
              $(DIST_REPOSITORY)/idl/cci/cciIStringWriter.idl \
              $(DIST_REPOSITORY)/include/daim/dmCoordinatesTypes.h \
              $(DIST_REPOSITORY)/include/daim/dmPixelFormats.h \
              $(DIST_REPOSITORY)/include/daim/dmImageData.h \
              $(DIST_REPOSITORY)/include/daim/dmOperators.h \
              $(DIST_REPOSITORY)/include/daim/dmRGBColor.h \
              $(NULL)

libs::
	@mkdir -p $(XPCOM_DIST)
	@rm -f $(XPCOM_DIST)/*.idl $(XPCOM_DIST)/*.h $(XPCOM_DIST)/filelist
	cp $(DIST_REPOSITORY)/idl/daim/*.idl $(XPCOM_DIST)/
	cp ccidaim.idl $(XPCOM_DIST)/
	for f in $(XPCOM_EXPORTS); do \
	  cp "$$f" $(XPCOM_DIST)/; \
	done
	cd $(XPCOM_DIST); ls *.idl > filelist
	
clean:: 
	@rm -f $(XPCOM_DIST)/*.idl $(XPCOM_DIST)/*.h $(XPCOM_DIST)/filelist
	
include $(DEPTH)/config/rules.mk
