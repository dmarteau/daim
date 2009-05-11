#
# Makefile for daim unit tests
#
DEPTH=../../..

include $(DEPTH)/config/config.mk

MODULE_NAME = tests
DAIM_TESTS  = test_glue \
              $(NULL)

libs::
	@for d in $(DAIM_TESTS); do \
		$(MAKE) -f test.mk TEST_NAME=$$d; \
	done

clean::
	@for d in $(DAIM_TESTS); do \
		$(MAKE) -f test.mk TEST_NAME=$$d clean; \
	done

include $(DEPTH)/config/rules.mk
