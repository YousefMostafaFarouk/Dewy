@echo off
setlocal
cd /d "%~dp0"

if not exist "C:\msys64\usr\bin\bash.exe" (
    echo MSYS2 was not found at C:\msys64.
    echo Install MSYS2 from https://www.msys2.org and try again.
    exit /b 1
)

set "CHERE_INVOKING=1"
set "MSYSTEM=UCRT64"
"C:\msys64\usr\bin\bash.exe" --login -c "./build-mingw.sh"
exit /b %errorlevel%
