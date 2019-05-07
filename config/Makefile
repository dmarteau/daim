#
# Global makefile for daim
#

# Include user/local configuration

DEPTH=..
include config.mk

BUILD_DIRS= \
  $(DEPTH)/include \
  $(DEPTH)/daim_utilities \
  $(DEPTH)/daim_matrix \
  $(DEPTH)/daim_libkernel \
  $(DEPTH)/daim_kernel \
  $(DEPTH)/cci \
  $(DEPTH)/daim \
  $(DEPTH)/utils \
  $(NULL)

.PHONY: all clean create_repository package config $(BUILD_DIRS)

all: create_repository $(BUILD_DIRS)

contrib:
	@$(MAKE) -C $(DEPTH)/contrib/sources

create_repository:
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIRS):
	@$(MAKE) -C $@ module

$(DEPTH)/daim_utilities: $(DEPTH)/include
$(DEPTH)/cci: $(DEPTH)/daim_utilities
$(DEPTH)/daim: $(DEPTH)/cci $(DEPTH)/daim_kernel
$(DEPTH)/daim_kernel: $(DEPTH)/daim_utilities $(DEPTH)/daim_matrix $(DEPTH)/daim_libkernel

package:
	@$(MAKE) -C $(DEPTH)/config -f packager.mk

clean:
	@for d in $(BUILD_DIRS); do \
		$(MAKE) -C $$d clean; \
	done

echo-config:
	@echo BUILD_CONFIG            = $(BUILD_CONFIG)
	@echo MAKE_TARGET             = $(MAKE_TARGET)
	@echo DAIM_DEBUG              = $(DAIM_DEBUG)
	@echo BUILD_DIR               = $(BUILD_DIR)
	@$(MAKE) -f $(MAKE_TARGET) config DEPTH=.. BUILD_CONFIG=$(BUILD_CONFIG)
