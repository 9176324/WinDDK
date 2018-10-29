
#
# Build classlog.h 
#

$(OBJ_PATH)\$O\classlog.bmf : $(OBJ_PATH)\$O\classlog.mof
$(OBJ_PATH)\$O\classlog.h: $(OBJ_PATH)\$O\classlog.bmf
    wmimofck -h$(OBJ_PATH)\$O\classlog.h  -x$(OBJ_PATH)\$O\classlog.x -u $(OBJ_PATH)\$O\classlog.bmf
    


