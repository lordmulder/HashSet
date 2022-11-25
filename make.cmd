@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"

if "%MSVC_PATH%"=="" (
	set "MSVC_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC"
)

if not exist "%MSVC_PATH%\Auxiliary\Build\vcvarsall.bat" (
	echo MSVC not found. Please check your MSVC_PATH and try again^^!
	pause
	goto:eof
)

for %%p in (x86,x64,ARM64) do (
	call "%MSVC_PATH%\Auxiliary\Build\vcvarsall.bat" %%p
	for %%c in (Static,Shared,Debug) do (
		echo.
		echo ------------------------------------------------------------------------------
		echo %%p %%c
		echo ------------------------------------------------------------------------------
		for %%t in (Clean,Rebuild,Build) do (
			MSBuild.exe /property:Configuration=%%c /property:Platform=%%p /target:%%t /verbosity:normal "%CD%\HashSet.sln"
			if not "!ERRORLEVEL!"=="0" goto:BuildFailed
		)
	)
)

echo.
echo Build completed successfully.
echo.
pause
goto:eof

:BuildFailed
echo.
echo Build has failed ^^!^^!^^!
echo.
pause
