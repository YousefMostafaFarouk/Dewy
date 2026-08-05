@echo off
setlocal
cd /d "%~dp0Dewy"

if not exist "Release\Dewy.exe" (
    echo Dewy has not been built yet. Run build.bat first.
    exit /b 1
)

"Release\Dewy.exe" %*
