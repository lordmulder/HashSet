@echo off
cd /d "%~dp0..\.."

set "MSYS2_DIR=C:\msys64"

call "%MSYS2_DIR%\msys2_shell.cmd" -mingw32 -no-start -defterm -where "%CD%" -c "./etc/utils/build-cygwin.sh"
call "%MSYS2_DIR%\msys2_shell.cmd" -mingw64 -no-start -defterm -where "%CD%" -c "./etc/utils/build-cygwin.sh"

pause
