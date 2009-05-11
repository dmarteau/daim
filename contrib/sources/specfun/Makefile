#
# Makefile for levmar routines
#

DEPTH= ../../..

include $(DEPTH)/config/config.mk

# Output distribution in contrib
MOD_PATH=contrib/

MODULE_NAME=specfun
MODULE_EXT =lib

MODULE_INCL=$(DEPTH)/contrib/include

# Don't want local directory to be a default include dir. 
NO_LOCAL_INCLUDE=1

MODULE_DEFS=NO_LONG_LONG

CMODULE_FILES= specfun.c
    
default: module

include $(DEPTH)/config/rules.mk
