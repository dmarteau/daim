DEPTH = ../../..

include $(DEPTH)/config/config.mk

ifdef DAIM_DEBUG
BUILD_MODE:=debug
else
BUILD_MODE:=release
endif

include $(DEPTH)/config/rules.mk

libs::
	mkdir -p "$(GDAL_OBJ_DIR)"
	cd "$(GDAL_SRC_DIR)" && cp -RLf * "$(GDAL_OBJ_DIR)"
	cmd.exe \/c "build.bat all $(BUILD_MODE) $(GDAL_OBJ_DIR) $(GDAL_DIST_DIR)"
