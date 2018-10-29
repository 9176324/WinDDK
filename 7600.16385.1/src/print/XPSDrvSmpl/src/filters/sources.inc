# Copyright (c) 2005 Microsoft Corporation
#
# All rights reserved.
#
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
# ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
# PARTICULAR PURPOSE.


INCLUDES=$(INCLUDES);         \
    ..\;                      \
    ..\common;                \
    ..\..\inc;                \
    ..\..\common;             \
    ..\..\debug;              \
    $(DDK_INC_PATH);          \
    $(SDK_INC_PATH)\gdiplus;  \

TARGETLIBS=$(TARGETLIBS)                     \
    $(SDK_LIB_PATH)\uuid.lib                 \
    $(SDK_LIB_PATH)\WIAGUID.lib              \
    $(SDK_LIB_PATH)\msxml6.lib               \
    $(SDK_LIB_PATH)\Kernel32.lib             \
    $(SDK_LIB_PATH)\ole32.lib                \
    $(SDK_LIB_PATH)\oleaut32.lib             \
    $(SDK_LIB_PATH)\prntvpt.lib              \
    $(SDK_LIB_PATH)\winspool.lib             \
    $(SDK_LIB_PATH)\gdi32.lib                \
    $(SDK_LIB_PATH)\gdiplus.lib              \
    $(SDK_LIB_PATH)\shlwapi.lib              \
    $(SDK_LIB_PATH)\Advapi32.lib             \
    $(SDK_LIB_PATH)\user32.lib               \
    $(OBJ_PATH)\..\..\debug\$O\xdsdbg.lib \
    $(OBJ_PATH)\..\..\common\$O\xdsmplcmn.lib \
    $(OBJ_PATH)\..\..\filters\common\$O\xdfltcmn.lib \

USE_ATL=1
USE_STL=1

ATL_VER=70
STL_VER=70

# We use STL 7.0, which is not available pre-Vista. We therefore set USE_LIBCMT=1
# If STL 7.0 is not necessary, USE_MSVCRT=1 is recommended
USE_LIBCMT=1

USE_IDLBASED_DLLDATA=1
USE_OBJECT_ROOT=1

MSC_WARNING_LEVEL=/W4 /WX

USE_NATIVE_EH=1

PRECOMPILED_CXX=1
PRECOMPILED_INCLUDE=..\precomp.h

C_DEFINES=$(C_DEFINES) -D_UNICODE -DUNICODE


