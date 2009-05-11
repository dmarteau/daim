#
# Makefile for libz routines
#

DEPTH= ../../..

include $(DEPTH)/config/config.mk

# Output distribution in contrib
MOD_PATH=contrib/

MODULE_NAME=z_s
MODULE_EXT =lib

CMODULE_FILES= \
       adler32.c compress.c crc32.c deflate.c gzio.c uncompr.c  trees.c \
       zutil.c inflate.c infback.c inftrees.c inffast.c

CONTRIB_HEADERS= zlib.h zconf.h

all: module finalize
	
finalize:
	@for d in $(CONTRIB_HEADERS); do \
		cp $$d ../../include/; \
	done

include $(DEPTH)/config/rules.mk

