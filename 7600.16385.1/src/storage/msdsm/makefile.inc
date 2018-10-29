#
# Copyright (C) 2004  Microsoft Corporation
#
# generate auxilary files from the binary mof (bmf) file
#
# msdsmwmi.h has definitions for the guids, data structures and definitions
#                of the method ids.
#
# msdsm.vbs is a ready to run vbscript applet that will query all classes
#            in the mof.
#
# msdsmdsm.h has definitions for the guids, data structures and definitions
#                of the DSM-centric classes.
#
# msdsmdsm.vbs is a ready to run vbscript applet that will query all classes
#            in the mof.

$(OBJ_PATH)\$O\msdsm.bmf: $(OBJ_PATH)\$O\msdsm.mof
$(OBJ_PATH)\$O\msdsm.vbs $(OBJ_PATH)\$O\msdsmwmi.h: $(OBJ_PATH)\$O\msdsm.bmf
    wmimofck -h$(OBJ_PATH)\$O\msdsmwmi.h -t$(OBJ_PATH)\$O\msdsm.vbs $(OBJ_PATH)\$O\msdsm.bmf
$(OBJ_PATH)\$O\msdsmdsm.bmf: $(OBJ_PATH)\$O\msdsmdsm.mof
$(OBJ_PATH)\$O\msdsmdsm.vbs $(OBJ_PATH)\$O\msdsmdsm.h: $(OBJ_PATH)\$O\msdsmdsm.bmf
    wmimofck -h$(OBJ_PATH)\$O\msdsmdsm.h -t$(OBJ_PATH)\$O\msdsmdsm.vbs $(OBJ_PATH)\$O\msdsmdsm.bmf


