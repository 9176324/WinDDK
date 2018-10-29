TARGETTYPE=DRIVER
DRIVERTYPE=WDM
TARGET_DESTINATION=Capture\SWTuner

INCLUDES= \
    $(DDK_INC_PATH); \
    $(DDK_INC_PATH)\wdm; \
    $(SDK_INC_PATH); \
    $(SDK_PATH)\AMovie\Inc; \
    $(MULTIMEDIA_INC_PATH); \
    $(INCLUDES)

TARGETLIBS= \
    $(DDK_LIB_PATH)\ks.lib \
    $(DDK_LIB_PATH)\ksguid.lib \
    $(DDK_LIB_PATH)\BdaSup.lib \
    $(DDK_LIB_PATH)\ntstrsafe.lib \

!if $(386)
C_DEFINES = $(C_DEFINES) -DX86
!elseif $(AMD64)
C_DEFINES = $(C_DEFINES) -DAMD64  -D_BUILD_SW_TUNER_ON_X64
!endif

SOURCES= \
    ..\ObjDesc.cpp  \
    ..\event.cpp    \
    ..\inpin.cpp    \
    ..\Filter.cpp   \
    ..\Device.cpp   \
    ..\bdaguid.c    \
    ..\registry.cpp \
    ..\CCapturePin.cpp  \
    ..\TStream.cpp  \
    ..\hwsim.cpp    \
    ..\fileread.cpp \
    ..\CTsTimestampModifier.cpp \
    ..\BDATuner.rc

