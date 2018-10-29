#############################################################################
#
#       Copyright (C) Microsoft Corporation. All Rights Reserved.
#
#       Sources for BDA sample generic analog tuner
#
##########################################################################

TARGETNAME=SWTANALOG
TARGETTYPE=DRIVER
DRIVERTYPE=WDM
TARGET_DESTINATION=Capture\SWTuner

INCLUDES= \
    $(DDK_INC_PATH); \
    $(DDK_INC_PATH)\wdm; \
    $(SDK_INC_PATH); \
    $(SDK_PATH)\AMovie\Inc; \
    $(INCLUDES)

TARGETLIBS= \
    $(DDK_LIB_PATH)\ks.lib \
    $(DDK_LIB_PATH)\ksguid.lib \
    $(DDK_LIB_PATH)\BdaSup.lib \
    
!if $(386)
C_DEFINES = $(C_DEFINES) -DX86
!elseif $(AMD64)
C_DEFINES = $(C_DEFINES) -DAMD64  -D_BUILD_SW_TUNER_ON_X64
!endif

SOURCES= \
    ..\device.cpp\
    ..\anlgtuner.cpp\
    ..\anlgevent.cpp\
    ..\xbar.cpp\
    ..\tvaudio.cpp\
    ..\anlgcapture.cpp\
    ..\encoder.cpp\
    ..\hwsim.cpp\
    ..\encoderpin.cpp\
    ..\tstream.cpp\
    ..\fileread.cpp\
    ..\registry.cpp \
    ..\anlgtuner.rc

