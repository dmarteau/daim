
DEPTH=../../..

include $(DEPTH)/config/config.mk

MODULE_INCL  = $(DEPTH)/include/unit_tests
MODULE_NAME  = tests
LIBRARY_NAME = $(TEST_NAME)
MODULE_EXT   = exe
MODULE_DEFS += DAIM_GLUE

MODULE_FILES=$(TEST_NAME).cpp

REQUIRES=cci daim

MODULE_LIBS = -ldaimglue -lcciglue 

ifeq ($(BUILD_CONFIG),-linux-gcc)
MODULE_LIBS += -ldl
endif

include $(topsrcdir)/config/rules.mk
