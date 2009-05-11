#=========================================================
# Makefile for daim libraries and modules
# The following should be defined in the module Makefile:
#
# MODULE_NAME    The name of the current module to compile
# MODULE_EXT     Output extension
# MODULE_INCL    Additional includes path
# MODULE_LIBS    Additional lib files
# MODULE_DEFS    Additional defines
# MODULE_FILES   List of source files to compile and link
#
# DIST_REPOSITORY  is defined in config/config.mk
# BUILD_REPOSITORY is defined in config/config.mk
#=========================================================

# Defines for lib version


MODULE_DEFS += __DAIM_LIB_VERSION__=$(DAIM_RUNTIME_VER_MAJOR).$(DAIM_RUNTIME_VER_MINOR)
MODULE_DEFS += __DAIM_LIB_REVISION__=$(DAIM_RUNTIME_VER_REVISION)

MODULE_DEFS += DAIM_RUNTIME_VER_MAJOR=$(DAIM_RUNTIME_VER_MAJOR)
MODULE_DEFS += DAIM_RUNTIME_VER_MINOR=$(DAIM_RUNTIME_VER_MINOR)
MODULE_DEFS += DAIM_RUNTIME_VER_REVISION=$(DAIM_RUNTIME_VER_REVISION)


# Use a specific package
ifdef MODULE_PACKAGE
  MOD_PATH      += apps/$(MODULE_PACKAGE)/
  MODULE_LIBDIR += $(DIST_REPOSITORY)/lib
endif

BINDIR=$(DIST_REPOSITORY)/$(MOD_PATH)bin
LIBDIR=$(DIST_REPOSITORY)/$(MOD_PATH)lib
CCIDIR=$(DIST_REPOSITORY)/$(MOD_PATH)modules
EXEDIR=$(DIST_REPOSITORY)/$(MOD_PATH)bin
INCDIR=$(DIST_REPOSITORY)/$(MOD_PATH)include
IDLDIR=$(DIST_REPOSITORY)/$(MOD_PATH)idl

MODULE_INCL += $(INCDIR)

ifdef LIBRARY_NAME
OBJDIR=$(BUILD_REPOSITORY)/.parts/$(LIBRARY_NAME)
endif

MODULE_LIBDIR += $(LIBDIR)

# Add the 'DEBUG' macros
ifeq ($(BUILD_DEBUG),1)
 MODULE_DEFS += DEBUG
endif

# Add contrib directory for libs path
MODULE_LIBDIR += $(DIST_REPOSITORY)/contrib/lib

# Set EXPORTS directory

ifndef MODULE_EXPORT_NAME
MODULE_EXPORT_NAME=$(MODULE_NAME)
endif

# If MODULE_EXPORT_NAME is defined but not set
# we export in the main include directory
ifeq ($(MODULE_EXPORT_NAME),)
  EXPORT_DIR=$(INCDIR)
  IDL_EXPORT_DIR=$(IDLDIR)
else
  EXPORT_DIR=$(INCDIR)/$(MODULE_EXPORT_NAME)
  IDL_EXPORT_DIR=$(IDLDIR)/$(MODULE_EXPORT_NAME)
endif


ifeq ($(MODULE_EXT),exe)
#=======================================
# Build an executable using daim library
#=======================================
MODULE_EXPORT = $(EXEDIR)/$(LIBRARY_NAME)$(EXESFX)
MODULE_EXE:=1

#=======================================
# Define path to the daim system relatives to bin directory
# This is because test programs must be executed with bin
# as working directory (otherwise dll names and modules names 
# won't be resolved).
#=======================================

#MODULE_DEFS += "_DAIM_USER=\"../tests/\""
#MODULE_DEFS += "_DAIM_LIB=\"../modules/\""

ifdef MODULE_ID
MODULE_DEFS += "_DAIM_APPID=\"$(MODULE_ID)\""
endif

else
#=======================================
# Build a daim component using core 
# libraries
#=======================================

ifeq ($(IS_COMPONENT),1)
MODULE_LIBRARY_NAME = $(LIBRARY_NAME).cci
MODULE_EXPORT = $(CCIDIR)/$(MODULE_NAME)/$(MODULE_LIBRARY_NAME)
MODULE_FILES += cciComponentGlue.cpp
VPATH += :$(DEPTH)/cci/glue
MODULE_SHARED:=1
REQUIRES += cci
MODULE_DEFS  += DAIM_GLUE 
MODULE_LIBS  += -lcciglue
endif

#ifeq ($(IS_STATIC_COMPONENT),1)
#  MODULE_EXT:=lib
#  MODULE_DEFS += CCI_TRANSLATE_MODULE_ENTRY_POINT
#  REQUIRES += cci
#endif

#=======================================
# Build a daim dynamic library
#=======================================

ifeq ($(MODULE_EXT),dll)
MODULE_LIBRARY_NAME = $(DLLPREFIX)$(LIBRARY_NAME)$(DLLSFX)
MODULE_EXPORT =$(BINDIR)/$(MODULE_LIBRARY_NAME)
MODULE_IMPORT =$(LIBDIR)/$(LIBPREFIX)$(LIBRARY_NAME)$(LIBSFX)
MODULE_SHARED:=1
endif

#=======================================
# Build a daim static library
#=======================================

ifeq ($(MODULE_EXT),lib)
MODULE_LIBRARY_NAME = $(LIBPREFIX)$(LIBRARY_NAME)$(LIBSFX)
MODULE_EXPORT = $(LIBDIR)/$(MODULE_LIBRARY_NAME)
endif

endif # MODULE_EXE

#=============================================
# Predefined setups for some general 
# configurations
#=============================================

# We link against daim dll
ifdef DAIM_LIB
MODULE_DEFS  += DAIM_LIB 
MODULE_LIBS  += -ldaim
endif

ifdef DAIM_LIBKERNEL
MODULE_LIBS += -ldaim_libkernel
DAIM_KERNEL:=1
endif

ifdef DAIM_KERNEL
MODULE_DEFS += DAIM_KERNEL
MODULE_LIBS += -ldaim_kernel
MODULE_INCL += $(DEPTH)/daim_kernel/include
DAIM_UTILITIES:=1
else
# Add exported daim_kernel files to include path
REQUIRES += daim_kernel
endif

ifdef DAIM_MATRIX
MODULE_LIBS += -ldaim_matrix
MODULE_INCL += $(DEPTH)/daim_matrix/include
DAIM_UTILITIES:=1
endif

ifdef DAIM_UTILITIES
MODULE_DEFS += DAIM_UTILITIES
MODULE_LIBS += -ldaim_utilities
MODULE_INCL += $(DEPTH)/daim_utilities/include
endif


#==============================================
# Build process and sub-processes
#==============================================

.PHONY: clean cleanup

# Always depends on platform
MODULE_INCL += $(INCDIR)/platform

# Add dependencies to include directories
ifdef REQUIRES
  MODULE_INCL += $(patsubst %,$(INCDIR)/%,$(REQUIRES))
#  $(error REQUIRES=$(REQUIRES) MODULE_INCL=$(MODULE_INCL))
endif

# Add module name to the include search
MODULE_INCL += $(INCDIR)/$(MODULE_NAME)

# Build/Clean only if we have something to build
ifdef MODULE_EXPORT
  BUILD=build
  CLEAN=cleanup
endif

# Add rules for making subdirs
ifdef DIRS
  CLEAN_SUBDIRS=clean_subdirs
endif


# Add rules for building idl headers
ifdef IDLSRC
BUILD_IDL=build_idl
CLEAN_IDL=clean_idl
include $(DEPTH)/config/idl.mk
endif


.PHONY: $(DIRS) $(BUILD_IDL)

module: export build_idl0 libs

libs:: $(DIRS) $(BUILD)

build_idl0: $(BUILD_IDL)
ifdef DIRS
	@for d in $(DIRS); do \
		$(MAKE) -C $$d build_idl0; \
	done
endif	

include $(MAKE_TARGET)

clean:: $(CLEAN_SUBDIRS) clean_deps $(CLEAN) $(CLEAN_IDL)

export::
ifdef MODULE_PACKAGE
	@echo "Building module for package $(MODULE_PACKAGE)"
endif
ifdef OBJDIR
	mkdir -p $(OBJDIR)
endif
	@mkdir -p $(BINDIR)
	@mkdir -p $(LIBDIR)
	@mkdir -p $(CCIDIR)
	@mkdir -p $(INCDIR)
	@mkdir -p $(IDLDIR)
ifdef IS_COMPONENT
	@mkdir -p $(CCIDIR)/$(MODULE_NAME)
endif
ifdef EXPORTS
	@mkdir -p $(EXPORT_DIR)
	@for f in $(EXPORTS); do \
		cp $(COPY_FLAGS) "$$f" "$(EXPORT_DIR)"; \
	done
endif
ifdef IDLSRC
	@mkdir -p $(EXPORT_DIR)
	@mkdir -p $(IDL_EXPORT_DIR)
	@for f in $(IDLSRC); do \
		cp $(COPY_FLAGS) "$$f" "$(IDL_EXPORT_DIR)"; \
	done
endif
ifdef DIRS
	@for d in $(DIRS); do \
		$(MAKE) -C $$d export; \
	done
endif

clean_deps:
ifdef EXPORTS
	@for f in $(EXPORTS); do \
	  rm -f "$(EXPORT_DIR)/$$f"; \
	done
endif
ifdef IDLSRC
	@for f in $(IDLSRC); do \
	  rm -f "$(IDL_EXPORT_DIR)/$$f"; \
	done
endif

ifdef DIRS
$(DIRS):
	@$(MAKE) -C $@ libs
endif

clean_subdirs:
	@for d in $(DIRS); do \
		$(MAKE) -C $$d clean; \
	done
