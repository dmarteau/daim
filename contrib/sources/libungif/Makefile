#
# Makefile for libpungif routines
#

DEPTH= ../../..

include $(DEPTH)/config/config.mk

# Output distribution in contrib
MOD_PATH=contrib/

MODULE_NAME=ungif_s
MODULE_EXT =lib

CMODULE_FILES= \
  dgif_lib.c egif_lib.c gif_err.c gifalloc.c quantize.c


CONTRIB_HEADERS= gif_lib.h

MODULE_DEFS = _GBA_NO_FILEIO

all: module finalizepng
	
finalizepng:
	@for d in $(CONTRIB_HEADERS); do \
		cp $$d ../../include/; \
	done

include $(DEPTH)/config/rules.mk

