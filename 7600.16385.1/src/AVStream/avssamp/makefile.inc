## Sign driver cat file after PASS2 (link)

!if "$(BUILD_PASS)" == "PASS2"

$(OBJ_PATH)\$(O)\avssamp.cat:
        copy /y avssamp.inf $(OBJ_PATH)\$(O)\avssamp.inf
	deltacat.cmd $(OBJ_PATH)\$(O)
	move /y $(OBJ_PATH)\$(O)\delta.cat $(OBJ_PATH)\$(O)\avssamp.cat
        move /y $(OBJ_PATH)\$(O)\delta.cdf $(OBJ_PATH)\$(O)\avssamp.cdf
        $(SIGNCODE_CMD) 
!else

$(OBJ_PATH)\$(O)\avssamp.cat:

!endif
