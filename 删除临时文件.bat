::@echo off 

@for /d /r %%c in (Listings, Objects) do @if exist %%c ( rd /s /q "%%c" & echo     删除目录%%c) 

@for /r  %%c in (*.Administrator,*.bak, *.windows, *.saved_uv4) do del "%%c"

pause