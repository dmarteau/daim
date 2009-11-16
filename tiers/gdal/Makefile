DEPTH = ../..

include $(DEPTH)/config/config.mk

ifeq ($(OS_ARCH),WINNT)
DIRS += windows
endif

export GDAL_DIST_DIR:=$(call normalizepath,$(shell cd "$(DIST_REPOSITORY)"; pwd)/gdal)
export GDAL_OBJ_DIR:=$(call normalizepath,$(shell cd "$(BUILD_REPOSITORY)"; pwd)/.parts/gdal)
export GDAL_SRC_DIR:=$(call normalizepath,$(shell cd "$(DEPTH)/../gdal"; pwd))

ifneq ($(OS_ARCH),WINNT)

GDAL_OPTIONS =  --with-threads=yes \
                --with-expat=no \
                --without-ogr \
                --with-sqlite3=no \
                --with-libz=internal \
                --with-png=internal \
                --with-libtiff=internal \
                --with-libgeotiff=internal \
                --with-jpeg=internal \
                --with-gif=internal \
                --without-curl \
                --with-hide-internal-symbols \
                --without-pam  \
                --without-bsb \
                --without-pcraster \
                --without-cfitsio \
                --without-netcdf \
                --without-ogdi \
                --without-fme \
                --without-mrsid \
                --without-jp2mrsid \
                --without-grib \
                --without-pg \
                --without-dwgdirect \
                --without-idb \
                --without-sde

ifdef DAIM_DEBUG
GDAL_OPTIONS += --enable-debug
else
#GDAL_OPTIONS += CFLAGS=-O2 CXXFLAGS=-O2
endif

libs::
	rsync -a -v --exclude .svn $(GDAL_SRC_DIR)/ $(GDAL_OBJ_DIR)
	cd $(GDAL_OBJ_DIR); ./configure --prefix=$(GDAL_DIST_DIR)/ \
	       --enable-shared=no --without-libtool --without-ld-shared --with-hide-internal-symbols \
	       $(GDAL_OPTIONS)
	$(MAKE) -j1 -C $(GDAL_OBJ_DIR) 
	$(MAKE) -j1 -C $(GDAL_OBJ_DIR) install
ifndef DAIM_DEBUG
	strip -S $(GDAL_DIST_DIR)/lib/libgdal.a
endif

clean::
	rm -rf $(GDAL_DIST_DIR)
	$(MAKE) -j1 -C $(GDAL_OBJ_DIR) clean;


endif

include $(DEPTH)/config/rules.mk

