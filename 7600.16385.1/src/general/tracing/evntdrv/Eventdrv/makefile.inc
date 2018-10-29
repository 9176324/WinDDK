$(O)\evntdrvEvents.h: evntdrv.xml
     $(MC) -km -h $(OBJ_PATH)\$(O) -W $(MC_WINMETAPATH) -r $(OBJ_PATH)\$(O) -z evntdrvEvents -b $**


