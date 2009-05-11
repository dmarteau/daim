#==============================================
# Configuration Makefile 
# TODO: Autoconf
#==============================================

DAIM_RUNTIME_VER_MAJOR    = 3
DAIM_RUNTIME_VER_MINOR    = 0
DAIM_RUNTIME_VER_REVISION = 0

 # Try to guess build config
ifeq ($(BUILD_CONFIG),)

ifeq ($(shell uname),Darwin)
  BUILD_CONFIG:=-darwin-gcc
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


# Add local configuration
include $(DEPTH)/config/setup.mk

# Ensure that we have a build config set
ifeq ($(BUILD_CONFIG),)
$(error BUILD_CONFIG not set !!)
endif

ifeq ($(BUILD_CONFIG),-win32-msvc)

  OS_ARCH = WINNT
  MAKE_TARGET=$(DEPTH)/config/Makefile.msvc.mk
  DLLPREFIX=
  LIBPREFIX=
  LIBSFX=.lib
  DLLSFX=.dll
  EXESFX=.exe

endif


ifeq ($(BUILD_CONFIG),-linux-gcc)

  OS_ARCH = LINUX
  MAKE_TARGET=$(DEPTH)/config/Makefile.gcc.mk
  DLLPREFIX=lib
  LIBPREFIX=lib
  LIBSFX=.a
  DLLSFX=.so
  EXESFX=
  
  OS_SUB_ARCH = UNIX
  
endif

ifeq ($(BUILD_CONFIG),-darwin-gcc)

  OS_ARCH = DARWIN
  MAKE_TARGET=$(DEPTH)/config/Makefile.darwin-gcc.mk
  DLLPREFIX=lib
  LIBPREFIX=lib
  LIBSFX=.a
  DLLSFX=.dylib
  
  OS_SUB_ARCH = UNIX

endif

# Hu ! No make target !!
ifeq ($(MAKE_TARGET),)
$(error Invalid target $(BUILD_CONFIG))
endif

BUILD_REPOSITORY = $(BUILD_DIR)
DIST_REPOSITORY  = $(BUILD_DIR)

# Special contrib directory
CONTRIB=$(DEPTH)/contrib


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

