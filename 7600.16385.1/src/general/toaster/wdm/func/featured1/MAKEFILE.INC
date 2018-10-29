$(OBJ_PATH)\$(O)\toaster.bmf : toaster.mof
    mofcomp -B:$@ toaster.mof
    wmimofck $@   



