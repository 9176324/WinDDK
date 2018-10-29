STAMP=stampinf -f $@ -d *

$(OBJ_PATH)\$(O)\netawifi.inf: .\netawifi.inx
    copy $(@B).inx $@
    $(STAMP)


