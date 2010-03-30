#
# Makefile for daim_dmatrix lib
#

DEPTH= ../..
VPATH= ../source

include $(DEPTH)/config/config.mk

MODULE_NAME=daim_dmatrix
LIBRARY_NAME=daim_dmatrix
MODULE_EXT =lib
MODULE_INCL=../include
MODULE_DEFS=DM_MATRIX_DOUBLE

DAIM_INTERNAL:=1

MODULE_FILES=dmMatrix.cpp \
             dmMatrixArray.cpp \
             dmMatrixBase.cpp \
             dmMatrixError.cpp \
             dmComplex.cpp \
             dmMatrixFT.cpp \
             dmMatrixFTImpl.cpp \
             dmMatrixFunc.cpp \
             dmMatrixLU.cpp \
             dmMatrixOperators.cpp \
             dmMatrixSG.cpp \
             dmMatrixType.cpp \
             dmMatrixRandom.cpp \
             dmRowCol.cpp \
             dmMatrixSVD.cpp \
             dmMatrixLSQ.cpp \
             dmMatrixFTVector.cpp \
             dmMatrixSpline.cpp \
             dmMatrixGaussJ.cpp

DAIM_UTILITIES:=1

include $(topsrcdir)/config/rules.mk
