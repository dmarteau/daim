#
# Makefile for daim_matrix lib
#

DEPTH= ..
VPATH= ./source

include $(DEPTH)/config/config.mk

MODULE_NAME=daim_matrix
LIBRARY_NAME=daim_matrix
MODULE_EXT =lib
MODULE_INCL=./include

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

DIRS = precision

include $(topsrcdir)/config/rules.mk

