#
# Makefile for libpng routines
#

DEPTH= ../../..

include $(DEPTH)/config/config.mk

# Output distribution in contrib
MOD_PATH=contrib/

MODULE_NAME=png_s
MODULE_EXT =lib

CMODULE_FILES= \
    png.c pngerror.c pnggccrd.c pngget.c pngmem.c \
	pngpread.c pngread.c pngrio.c pngrtran.c pngrutil.c \
	pngset.c pngtrans.c pngvcrd.c pngwio.c pngwrite.c \
	pngwtran.c pngwutil.c

CONTRIB_HEADERS= png.h pngconf.h

all: module finalizepng
	
finalizepng:
	@for d in $(CONTRIB_HEADERS); do \
		cp $$d ../../include/; \
	done

include $(DEPTH)/config/rules.mk

