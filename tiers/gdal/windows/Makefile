DEPTH		= ../../..
topsrcdir	= @top_srcdir@
srcdir		= @srcdir@
VPATH		= @srcdir@

include $(DEPTH)/config/autoconf.mk

ifdef DAIM_DEBUG
BUILD_MODE:=debug
else
BUILD_MODE:=release
endif

include $(topsrcdir)/config/rules.mk

libs::
	$(RSYNC) --exclude .svn $(GDAL_SRC_DIR)/ $(GDAL_OBJ_DIR)
	cmd.exe \/c "build.bat all $(BUILD_MODE) $(GDAL_OBJ_DIR) $(GDAL_DIST_DIR)"
