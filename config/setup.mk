#==============================================
# Configuration Makefile 
#
# TODO: Autoconf
#==============================================

DAIM_RUNTIME_VER_MAJOR    = 3
DAIM_RUNTIME_VER_MINOR    = 0
DAIM_RUNTIME_VER_REVISION = 0

ifndef BUILD_DEBUG
BUILD_DEBUG:=1
endif

ifdef DAIM_BUILD_CONFIG
BUILD_CONFIG=$(DAIM_BUILD_CONFIG)
endif


BUILD_VERSION=$(shell sh -c 'date +"20%y%m%d%H"' )

# Build unit tests
BUILD_TESTS:=1

#=============================================
# Set the following options in <mydaimoption>.mk
# And set the variable DAIM_BUILD_OPTIONS to 
# the location of that file
#=============================================

# GDAL driver config

# Use gdal driver (gdal required)
DM_USE_GDAL=1

# Use custom build of gdal
GDAL_INTERNAL=1

# Link dynamically with gdal (enable full gdal capabilities)
#DM_GDAL_DLL=1

# Load user options that override defaults settings
ifdef DAIM_BUILD_OPTIONS
include $(DAIM_BUILD_OPTIONS)
endif

#==========================================
# Define output object dir if not already defined
#==========================================
ifndef DAIM_OBJ_DIR

ifeq ($(BUILD_DEBUG),1)
BUILD_DIR=$(DEPTH)/../bin$(BUILD_CONFIG)-debug
else
BUILD_DIR=$(DEPTH)/../bin$(BUILD_CONFIG)-release
endif

else
#BUILD_DIR=$(call normalizepath,$(DAIM_OBJ_DIR))
BUILD_DIR=$(DAIM_OBJ_DIR)
endif

