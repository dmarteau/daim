#==============================================
# Configuration Makefile 
# TODO: Autoconf
#==============================================

DAIM_RUNTIME_VER_MAJOR    = 3
DAIM_RUNTIME_VER_MINOR    = 1
DAIM_RUNTIME_VER_REVISION = 0

ifdef DAIM_BUILD_CONFIG
BUILD_CONFIG=$(DAIM_BUILD_CONFIG)
endif

 # Try to guess build config
ifeq ($(BUILD_CONFIG),)

ifeq ($(shell uname),Darwin)

ifeq ($(DAIM_USE_CLANG),1)
    BUILD_CONFIG:=-darwin-clang
else
    BUILD_CONFIG:=-darwin-gcc
endif

endif

ifeq ($(shell uname),Linux)
  BUILD_CONFIG:=-linux-gcc
endif

# Fallback to windows build
ifeq ($(BUILD_CONFIG),)
  BUILD_CONFIG:=-win32-msvc
# Check for cygwin
ifeq ($(shell uname -o),Cygwin)
CYGWIN_WRAPPER:=1
endif
  
endif 

endif

ifndef topsrcdir
topsrcdir = $(shell cd $(DEPTH); pwd)
endif

#
# Personal makefile customizations go in these optional make include files.
#
MY_CONFIG := $(DEPTH)/config/myconfig.mk
MY_RULES  := $(DEPTH)/config/myrules.mk

# Build unit tests
BUILD_TESTS:=1

#=============================================
# Set the following options in myconfig.mk
# And set the variable DAIM_BUILD_OPTIONS to 
# the location of that file
#=============================================

# GDAL driver config

# Use gdal driver (gdal required)
ifndef DAIM_GDAL
DAIM_GDAL=1
endif

#
# Include user/app overrides
#
-include $(MY_CONFIG)

#==========================================
# Define output object dir if not already defined
#==========================================
ifndef DAIM_OBJ_DIR

ifdef DAIM_DEBUG
BUILD_DIR=$(shell cd $(topsrcdir)/../;pwd)/bin$(BUILD_CONFIG)-debug
else
BUILD_DIR=$(shell cd $(topsrcdir)/../;pwd)/bin$(BUILD_CONFIG)-release
endif

else
BUILD_DIR=$(DAIM_OBJ_DIR)
endif

#=======================================
# Build configuration
#=======================================

# Ensure that we have a build config set
ifeq ($(BUILD_CONFIG),)
$(error BUILD_CONFIG not set !!)
endif

ifeq ($(BUILD_CONFIG),-win32-msvc)

  OS_ARCH = WINNT
  MAKE_TARGET=$(topsrcdir)/config/Makefile.msvc.mk
  DLLPREFIX=
  LIBPREFIX=
  LIBSFX=.lib
  DLLSFX=.dll
  EXESFX=.exe
  
endif


ifeq ($(BUILD_CONFIG),-linux-gcc)

  OS_ARCH = LINUX
  MAKE_TARGET=$(topsrcdir)/config/Makefile.gcc.mk
  DLLPREFIX=lib
  LIBPREFIX=lib
  LIBSFX=.a
  DLLSFX=.so
  EXESFX=
  
  OS_SUB_ARCH = UNIX
  
endif

ifeq ($(BUILD_CONFIG),-darwin-gcc)

  MAKE_TARGET=$(topsrcdir)/config/Makefile.darwin-gcc.mk
  DLLPREFIX=lib
  LIBPREFIX=lib
  LIBSFX=.a
  DLLSFX=.dylib
  
  OS_SUB_ARCH = UNIX

ifeq ($(OSX_FRAMEWORK),1)
ifndef OSX_FRAMEWORK_PREFIX
  OSX_FRAMEWORK_PREFIX = /Library/Frameworks/DAIM.framework
endif
  OSX_VERSION_FRAMEWORK_PREFIX = $(OSX_FRAMEWORK_PREFIX)/Versions/$(DAIM_RUNTIME_VER_MAJOR).$(DAIM_RUNTIME_VER_MINOR)
endif

endif

ifeq ($(BUILD_CONFIG),-darwin-clang)

  MAKE_TARGET=$(topsrcdir)/config/Makefile.darwin-clang.mk
  DLLPREFIX=lib
  LIBPREFIX=lib
  LIBSFX=.a
  DLLSFX=.dylib
  
  OS_SUB_ARCH = UNIX

ifeq ($(OSX_FRAMEWORK),1)
ifndef OSX_FRAMEWORK_PREFIX
  OSX_FRAMEWORK_PREFIX = /Library/Frameworks/DAIM.framework
endif
  OSX_VERSION_FRAMEWORK_PREFIX = $(OSX_FRAMEWORK_PREFIX)/Versions/$(DAIM_RUNTIME_VER_MAJOR).$(DAIM_RUNTIME_VER_MINOR)
endif

endif


# Hu ! No make target !!
ifeq ($(MAKE_TARGET),)
$(error Invalid target $(BUILD_CONFIG))
endif

# Special contrib directory
CONTRIB=$(topsrcdir)/contrib


ifeq ($(OS_ARCH),WINNT)
ifdef CYGWIN_WRAPPER
normalizepath = $(foreach p,$(1),$(shell cygpath -w $(p)))
else
# assume MSYS
#  We use 'pwd -W' to get DOS form of the path.  However, since the given path
#  could be a file or a non-existent path, we cannot call 'pwd -W' directly
#  on the path.  Instead, we extract the root path (i.e. "c:/"), call 'pwd -W'
#  on it, then merge with the rest of the path.
root-path = $(shell echo $(1) | sed -e "s|\(/[^/]*\)/\?\(.*\)|\1|")
non-root-path = $(shell echo $(1) | sed -e "s|\(/[^/]*\)/\?\(.*\)|\2|")
dos-path      = $(foreach p,$(1),$(if $(filter /%,$(p)),$(shell cd $(call root-path,$(p)) && pwd -W)$(call non-root-path,$(p)),$(p)))
# Normalize to windows path (i.e c:\)
normalizepath = $(subst /,\,$(call dos-path,$(1)))
endif
else
normalizepath = $(1)
endif

