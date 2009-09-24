#!/bin/sh

# Minimal gdal build for static link with Daim driver

GDAL_COMMAND=$1
GDAL_BUILDTYPE=$2
GDAL_SRC_DIR=$3
GDAL_INSTALLDIR=$4

echo "command=$GDAL_COMMAND mode=$GDAL_BUILDTYPE srcdir=$GDAL_SRC_DIR installdir=$GDAL_INSTALLDIR"

if [ -d $GDAL_SRC_DIR ];then
	cd $GDAL_SRC_DIR
else
	echo "$GDAL_SRC_DIR does not exists !"
	exit 20
fi


case $GDAL_BUILDTYPE in
debug)
	ENABLE_DEBUG=--enable-debug
	;;
release)
	;;
*)
	echo "usage gdal-make command [debug|release|]"
	exit 20
	;;
esac

do_configure ()
{
  ./configure --prefix=$GDAL_INSTALLDIR/ $ENABLE_DEBUG --enable-shared=no --without-libtool  --without-ld-shared \
                --with-threads=yes \
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
}

do_build()
{
	make
}

do_install()
{
	make install
	if ["_$GDAL_BUILDTYPE" = "_release"]; then
		strip -S $GDAL_BUILDDIR/lib/libgdal.a
	fi
}

do_clean()
{
	make clean
}

# check the command
case $1 in
configure)
	do_configure
	;;
build)
	do_build
	;;
install)
	do_install
	;;
clean)
	do_clean
	;;
all)
    do_clean
	do_configure
	do_build
	do_install
	;;
*)
	echo "$1 : unknow command" 
	;;
esac


