#

DEPTH= ../../../../../../..

include $(DEPTH)/config/config.mk

ifeq ($(BUILD_DEBUG),1)
GDAL_BUILDTYPE:=debug
else
GDAL_BUILDTYPE:=release
endif

$(shell chmod 755 gdal-make.sh)

GDAL_INSTALL_DIR:=$(call normalizepath,$(shell cd "$(DIST_REPOSITORY)"; pwd)/gdal)

ifndef GDAL_SRC_DIR
GDAL_SRC_DIR:=$(shell cd $(DEPTH)/../gdal; pwd)
endif

_GDAL_SRC_DIR:=$(call normalizepath,$(GDAL_SRC_DIR))

GDAL_BUILD_ARGS=$(GDAL_BUILDTYPE) $(_GDAL_SRC_DIR) $(GDAL_INSTALL_DIR)

ifeq ($(OS_ARCH),WINNT)

libs::
	if [ ! -d $(DIST_REPOSITORY)/gdal ] ; then \
	   cmd.exe \/c "windows\gdal-make.bat all $(GDAL_BUILD_ARGS)"; \
	fi

clean::
	cmd.exe \/c "windows\gdal-make.bat clean  $(GDAL_BUILD_ARGS)"
	rm -fr "$(GDAL_INSTALL_DIR)"

else

libs::
	if [ ! -d $(GDAL_INSTALL_DIR) ] ; then \
		./gdal-make.sh all $(GDAL_BUILD_ARGS); \
	fi

clean::
	./gdal-make.sh clean  $(GDAL_BUILD_ARGS)
	rm -fr $(GDAL_INSTALL_DIR)

endif


include $(DEPTH)/config/rules.mk
