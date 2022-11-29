@echo off
cd /d "%~dp0"

if "%PANDODC_PATH%"=="" (
	set "PANDODC_PATH=c:\Program Files (x86)\Pandoc"
)

echo on
"%PANDODC_PATH%\pandoc.exe" -o "%CD%\README.html" --self-contained --metadata title="LibHashSet" --toc --toc-depth=3 --css etc\style\gh-pandoc.min.css "%CD%\README.md"
@echo off

echo.
pause
