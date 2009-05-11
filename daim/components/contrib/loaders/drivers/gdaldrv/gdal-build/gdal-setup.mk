#

DEPTH= ../../../../../../..

include $(DEPTH)/config/config.mk


# If GDAL_HOME si not set 
# build our own version of gdal
ifeq ($(GDAL_INTERNAL),1)

ifeq ($(OS_ARCH),WINNT)


GDAL_SRC_DIR=$(DEPTH)/../gdal

SRCDIR=$(call normalizepath,$(shell pwd))

PATCH_FILES= \
     windows/nmake_patch.txt \
     windows/makefile_patch.txt


ifndef DM_GDAL_DLL
	_GDAL_APPLY_PATCH = cd $(GDAL_SRC_DIR);$(foreach p,$(PATCH_FILES),patch < "$(SRCDIR)/$(p)";) echo 
endif

endif

GDAL_SVN=https://svn.osgeo.org/gdal/branches/1.5/gdal

all:
	@echo Retrieving GDAL library from $(GDAL_SVN)
	cd $(DEPTH)/../; mkdir -p gdal; svn co $(GDAL_SVN) gdal
	$(_GDAL_APPLY_PATCH)

else

all:
	@echo "DAIM use external GDAL configuration"

endif
