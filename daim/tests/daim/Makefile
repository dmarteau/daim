#
# Makefile for daim unit tests
#
DEPTH=../../..

include $(DEPTH)/config/config.mk

MODULE_NAME = tests
DAIM_TESTS  = test_kernelmngr \
              test_registry \
              test_loaders \
              test_exif \
              test_imageshell \
              test_downsampling \
              test_sharpen \
              $(NULL)

libs::
	@for d in $(DAIM_TESTS); do \
		$(MAKE) -f test.mk TEST_NAME=$$d; \
	done

clean::
	@for d in $(DAIM_TESTS); do \
		$(MAKE) -f test.mk TEST_NAME=$$d clean; \
	done

include $(topsrcdir)/config/rules.mk


