#   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
#   ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
#   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
#   PARTICULAR PURPOSE.
#
#   Copyright (c) Microsoft Corporation. All rights reserved
#
#   File Name:
#
#      makefile.inc
#
#   Abstract:
#
#      Adds a custom target to copy the built DLL into the install directory.
#

INSTALLDIR=..\install

COPYDLL:
        if not exist $(INSTALLDIR)\$(_BUILDARCH) (md $(INSTALLDIR)\$(_BUILDARCH) )
        if exist $(OBJ_PATH)\$(O)\$(TARGETNAME).dll copy $(OBJ_PATH)\$(O)\$(TARGETNAME).dll $(INSTALLDIR)\$(_BUILDARCH)


