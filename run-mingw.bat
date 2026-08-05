@echo off
setlocal
cd /d "%~dp0build-mingw"

if not exist "Dewy.exe" (
    echo The MinGW build does not exist. Run build-mingw.bat first.
    exit /b 1
)

"Dewy.exe" %*
