::@echo off 

@for /r  %%c in (*.crf, *.map, *.lst, *.d, *.o, *.axf, *.htm, *.sct, *.lnp, *.hex, *.dep, *.__i, *.scvd, *.bak) do del "%%c"

pause