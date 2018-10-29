$(OBJ_PATH)\$(O)\pcidrv.bmf : ..\pcidrv.mof
    mofcomp -B:$@ ..\pcidrv.mof
    wmimofck $@   



