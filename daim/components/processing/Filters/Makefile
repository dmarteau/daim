#

DEPTH= ../../../..

include $(DEPTH)/config/config.mk

MODULE_NAME=daim
LIBRARY_NAME=filters

MODULE_FILES=cciConvolutionFilter.cpp \
			 cciRFilter.cpp \
			 cciLFilter.cpp \
             cciFiltersModule.cpp \
             cciSharpenFilter.cpp \
             $(NULL)

IS_COMPONENT=1

DIRS = public

MODULE_LIBS=-ldaimglue

DAIM_LIBKERNEL:=1

include $(DEPTH)/config/rules.mk
