#
# Makefile for daim dll
#

DEPTH= ../../..

include $(DEPTH)/config/config.mk

MODULE_NAME = xpcom

XPCOM_DIST=$(BUILD_DIR)/xpcom

XPCOM_EXPORTS=$(DEPTH)/cci/xpcom/cciIXPCOMSupports.idl \
              $(BUILD_DIR)/idl/cci/cciIMemory.idl \
              $(BUILD_DIR)/idl/cci/cciIStringIterator.idl \
              $(BUILD_DIR)/include/daim/cciDaimCID.h \
              $(BUILD_DIR)/include/daim/dmCoordinatesTypes.h \
              $(BUILD_DIR)/include/daim/dmPixelFormats.h \
              $(BUILD_DIR)/include/daim/dmImageData.h \
              $(BUILD_DIR)/include/daim/dmOperators.h \
              $(BUILD_DIR)/include/daim/dmRGBColor.h \
              $(NULL)

libs::
	@mkdir -p $(XPCOM_DIST)
	@rm -f $(XPCOM_DIST)/*.idl $(XPCOM_DIST)/*.h $(XPCOM_DIST)/filelist
	cp $(BUILD_DIR)/idl/daim/*.idl $(XPCOM_DIST)/
	cp ccidaim.idl $(XPCOM_DIST)/
	for f in $(XPCOM_EXPORTS); do \
	  cp "$$f" $(XPCOM_DIST)/; \
	done
	cd $(XPCOM_DIST); ls *.idl > filelist
	
clean:: 
	@rm -f $(XPCOM_DIST)/*.idl $(XPCOM_DIST)/*.h $(XPCOM_DIST)/filelist
	
include $(topsrcdir)/config/rules.mk
