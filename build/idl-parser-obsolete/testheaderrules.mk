# Copy this to objdir/myrules.mk to test

ifneq ($(XPIDLSRCS)$(SDK_XPIDLSRCS),)
export:: $(patsubst %.idl,$(XPIDL_GEN_DIR)/%.pyh, $(XPIDLSRCS) $(SDK_XPIDLSRCS))
endif

$(XPIDL_GEN_DIR)/%.pyh: %.idl $(XPIDL_GEN_DIR)/%.h /builds/idl-parser/header.py /builds/idl-parser/xpidl.py
	python -O /builds/idl-parser/header.py --cachedir=$(DEPTH)/config $(XPIDL_FLAGS) $< > $@
	diff -w -B -U 3 $(XPIDL_GEN_DIR)/$*.h $@
