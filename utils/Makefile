
DEPTH= ..

include $(DEPTH)/config/config.mk

MODULE_NAME = utils

# Print image data need "getop.h" not provided withe visual
ifneq ($(OS_ARCH),WINNT)
DIRS += printimagedata
endif

include $(DEPTH)/config/rules.mk

.PHONY: build_tests
