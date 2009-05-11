
DEPTH= ../..

include $(DEPTH)/config/config.mk

MODULE_NAME  = xpcom

XPCOM_DIST=$(DIST_REPOSITORY)/xpcom

XPCOM_EXPORT = $(DIST_REPOSITORY)/idl/cci/cciIStringWriter.idl \
               cciIXPCOMSupports.idl \
               $(NULL)
  
libs::
	@mkdir -p $(XPCOM_DIST)
	@cp $(XPCOM_EXPORT) $(XPCOM_DIST)
	
clean:: 
	@rm -f $(XPCOM_DIST)/cciIXPCOMSupports.idl
	@rm -f $(XPCOM_DIST)/cciIStringWriter.idl

include $(DEPTH)/config/rules.mk
