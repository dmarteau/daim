#!/bin/bash

#
# This script checkout all necessary ressources and build daim
# on linux and macosx platform
#

BUILDCOMMAND=$1
BUILDMODE=$2
BUILDDIR=$3

SRCDIR=$(pwd)

function print_usage
{
	echo "usage daim-setup [co|update|build] [debug|release] [builddir]"
}

function safexec
{
	$@
	if [ $? -ne 0 ]; then
		echo "Error, exiting..."
		exit 1
	fi
}

function do_checkout
{
	echo "Creating daim directories"

    cd $SRCDIR
	mkdir -p src
	echo "Checkout daim..."
	safexec svn co https://zoomodev.com/dev/svn/daim/trunk/daim  src
}

function do_update
{
	echo "Updating daim library"
	
	safexec cd $SRCDIR/src/
	safexec svn up
}

function do_build
{
	echo "Building daim library"

    case $BUILDMODE in
    release)
  	  export BUILD_DEBUG=0 
  	  ;;
    *)
      BUILDMODE=debug
      export BUILD_DEBUG=1
    ;;
    esac
	
	safexec cd $SRCDIR/src/
	
	safexec make -kf build.mk clean
	safexec make -f  build.mk build
	safexec make -f  build.mk register
}


case $BUILDCOMMAND in
co)
	do_checkout
	;;
update)
    do_update
	;;
build)
	if [ "_$BUILDDIR" != "_" ] ; then
		export DAIM_OBJ_DIR=$BUILDDIR
		echo "Buidlding in $BUILDDIR"
		if [ ! -d $BUILDDIR ] ; then
			mkdir -p $BUILDDIR
		fi
	fi

    do_build
	;;
*)
    print_usage
	exit 20
	;;
esac
