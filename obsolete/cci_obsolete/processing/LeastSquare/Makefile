#
# Makefile for lstsqr.cci
#

DEPTH=../../..

include $(DEPTH)/config/config.mk

MODULE_NAME=lstsqr
MODULE_EXT=cci
MODULE_GROUP=processing/
MODULE_FILES=CCI_LeastSquare.cpp
MODULE_DEFS=DM_MATRIX_DOUBLE

DAIM_UTILITIES:=1

MODULE_LIBS = -ldaim_dmatrix
MODULE_INCL = $(DEPTH)/daim_matrix/include

include $(DEPTH)/config/rules.mk
