TARGETTYPE=DRIVER

TARGETLIBS=$(DDK_LIB_PATH)\ndis.lib

MSC_WARNING_LEVEL=/WX /W4

# System and NDIS wrapper definitions.
C_DEFINES=$(C_DEFINES) -DNDIS_MINIPORT_DRIVER=1
C_DEFINES=$(C_DEFINES) -DNDIS_WDM=1 

# disabled warning 4201 -- nonstandard extension used : nameless struct/union
# disabled warning 4214 -- nonstandard extension used : bit field types other than int
# disabled warning 4127 -- conditional expression is constant
MSC_WARNING_LEVEL=$(MSC_WARNING_LEVEL) /wd4201 /wd4214 /wd4127


SOURCES = \
    ..\miniport.c \
    ..\adapter.c \
    ..\ctrlpath.c \
    ..\datapath.c \
    ..\tcbrcb.c \
    ..\mphal.c \

RUN_WPP= \
    $(SOURCES) \
    -km \
    -func:DEBUGP(LEVEL,MSG,...)
