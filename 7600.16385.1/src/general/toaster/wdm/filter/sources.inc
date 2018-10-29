TARGETTYPE=DRIVER

MSC_WARNING_LEVEL= /W4 /WX

TARGETLIBS=  $(DDK_LIB_PATH)\wdmsec.lib \
             $(DDK_LIB_PATH)\rtlver.lib \
             
SOURCES= ..\filter.c \
	 ..\filter.rc


