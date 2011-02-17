#

DEPTH= ../..

include $(DEPTH)/config/config.mk

MODULE_NAME=daim

IDLSRC = ccidaim.idl \
         cciIImageList.idl \
         cciISurface.idl \
         cciIDaimService.idl \
         cciIMatrix.idl \
         cciIMetaDataContainer.idl \
         cciIImageContainer.idl \
         cciIRegionContainer.idl \
         cciIRegion.idl \
         cciIFilterMask.idl \
         cciIImageFilter.idl \
         cciIKernelFamily.idl \
         cciIKernelFamilyContainer.idl \
         cciIKernelService.idl \
         cciIScriptableKernelFamily.idl \
         cciIColorSpace.idl \
         cciIFilterContext.idl \
         cciIImageShell.idl \
         cciIColorTable.idl \
         cciIImage.idl \
         cciIImageListImage.idl \
         cciIStorageImage.idl \
         $(NULL)

EXPORTS = cciDaimCID.h \
          cciIImageListContainer.h \
          cciKernelServiceUtils.h \
          $(NULL)

include $(topsrcdir)/config/rules.mk
