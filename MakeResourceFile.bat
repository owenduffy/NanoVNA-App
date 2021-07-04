
REM Run this to create the data.res file.
REM
REM This will cause the list of files listed in data.rc to be added into the .exe when it's compiled.

brcc32.exe -v -fodata.res data.rc
pause

