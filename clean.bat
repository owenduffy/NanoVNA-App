
REM Clean up this directory .. removes the left over (un-needed) files that were created during the compiling of the .exe file.

rmdir /s /q ".\__recovery"
rmdir /s /q ".\__history"

del /s *.obj *.tds *.dsk *.map *.drc *.cgl *.stat *.local

del /s .\Win32\Debug\*.#*
del /s .\Win32\Debug\*.res
del /s .\Win32\Debug\*.ilc
del /s .\Win32\Debug\*.ild
del /s .\Win32\Debug\*.ilf
del /s .\Win32\Debug\*.ils
del /s .\Win32\Debug\*.pdi
del /s .\Win32\Debug\*.pch

del /s .\Win32\Release\*.#*
del /s .\Win32\Release\*.res
del /s .\Win32\Release\*.ilc
del /s .\Win32\Release\*.ild
del /s .\Win32\Release\*.ilf
del /s .\Win32\Release\*.ils
del /s .\Win32\Release\*.pdi
del /s .\Win32\Release\*.pch

::pause
