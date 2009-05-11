

# Resolve defs includes path and lib files
MODULE_DEFS  :=$(patsubst %,-D%,$(MODULE_DEFS)) 
MODULE_INCL  :=$(patsubst %,-I%,$(MODULE_INCL))
MODULE_LIBDIR:=$(patsubst %,-L%,$(MODULE_LIBDIR))

ifndef NO_LOCAL_INCLUDE
LOCAL_INCLUDE = -I.
endif

MODULE_LIBDIR += -L$(BINDIR)

#========================
# Common compile options
#========================

ifdef GCC_VERSION
  GCC_OPT_VERSION = -V $(GCC_VERSION)
endif

CC       = g++ $(GCC_OPT_VERSION)
LD       = g++ $(GCC_OPT_VERSION)
AR       = ar -rcvs
RANLIB   = ranlib
INCL     = $(LOCAL_INCLUDE) $(MODULE_INCL)  
DEFS     = $(MODULE_DEFS)
OPT      = -Wall -Wno-non-virtual-dtor 
CCFLAGS  = $(OPT) -fno-rtti $(INCL) $(DEFS)
CFLAGS   = $(OPT) $(INCL) $(DEFS)
LIBS     = $(MODULE_EXTRA_LIBS) $(MODULE_LIBS) 
LDFLAGS  = -L. $(MODULE_LIBDIR)

# GCC4 visibility
CCFLAGS += -fvisibility-inlines-hidden -fvisibility=hidden

#=====================
# Debug options
#=====================

ifeq ($(BUILD_DEBUG),1)
 OPT  += -g
 DEFS += -D_DEBUG
else
 OPT += -O2
endif

# We need to set operator new as not exported because 
# 1. Sometime only one of operator new/delete is exported and
# that cause a big mess in client code
# 2. Library may be used in client code where custom definition of operator new/delete
# has to be avoided

# linker tend to export too many symbols, components should only export
# one symbol "__CCI_Module"  

#LDFLAGS  += -Wl,-rpath,./

ifdef MODULE_EXE
#=====================
# Options for executable
#=====================

else # MODULE_EXE

DEFS += -DDM_THROW_BAD_ALLOC_SIG

ifdef MODULE_SHARED

#=====================
# Options for shared dll
#=====================

CCFLAGS += -fPIC 
 
# GCC4 visibility
CCFLAGS += -Wno-strict-aliasing

ifeq ($(IS_COMPONENT),1)
LDFLAGS += -Wl,-exported_symbol,__CCI_Module
LDFLAGS += -bundle

ifeq ($(MODULE_EXPORT_NAME),)
DYLD_FILE_PATH=../bin
else
DYLD_FILE_PATH=../../bin
endif

else
# Don't export new/delete custom symbols 
LDFLAGS += -unexported_symbols_list $(DEPTH)/config/unexported.exp
LDFLAGS += -dynamiclib
LDFLAGS += -install_name @loader_path/$(MODULE_LIBRARY_NAME) 
endif

MODULE_EXE:=1

endif # MODULE_SHARED
endif # MODULE_EXE

OBJECTS  = $(patsubst %.cpp,$(OBJDIR)/%.o,$(MODULE_FILES))
OBJECTS += $(patsubst %.c,$(OBJDIR)/%.o,$(CMODULE_FILES))

build: $(MODULE_EXPORT)

$(OBJDIR)/%.o : %.cpp
	$(CC) $(CCFLAGS) -c -o $@ $<


$(OBJDIR)/%.o : %.c
	gcc $(CFLAGS) -c -o $@ $<

$(MODULE_EXPORT): $(OBJECTS)
ifdef MODULE_EXE
	$(LD) $(LDFLAGS) -o $(MODULE_EXPORT) $(OBJECTS) $(MODULE_STATIC_LIBS) $(LIBS) 
ifeq ($(IS_COMPONENT),1)
ifdef DAIM_UTILITIES
	install_name_tool -change @loader_path/$(DLLPREFIX)daim_utilities$(DLLSFX) @loader_path/$(DYLD_FILE_PATH)/$(DLLPREFIX)daim_utilities$(DLLSFX) $@
endif
ifdef DAIM_KERNEL
	install_name_tool -change @loader_path/$(DLLPREFIX)daim_kernel$(DLLSFX) @loader_path/$(DYLD_FILE_PATH)/$(DLLPREFIX)daim_kernel$(DLLSFX) $@
endif
endif
else
	$(AR) $@ $(OBJECTS)
	$(RANLIB) $@
endif

cleanup:
	@rm -fr $(OBJDIR)
	@rm -f $(MODULE_EXPORT)


config:
	@echo Compiler configuration:
	@echo PATH=$(PATH)
	@echo LIB="$(LIB)"
	@echo INCLUDE="$(INCLUDE)"
	@echo LD=$(LD)
	@$(CC) --version

