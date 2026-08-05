@echo off
setlocal
cd /d "%~dp0"

set "dewy_msbuild=msbuild"
where msbuild >nul 2>nul
if errorlevel 1 (
    if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" (
        for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do set "dewy_msbuild=%%i"
    ) else (
        echo Visual Studio 2022 with Desktop development with C++ was not found.
        exit /b 1
    )
)

"%dewy_msbuild%" Dewy.sln /m /p:Configuration=Release /p:Platform=x64
if errorlevel 1 exit /b %errorlevel%

echo.
echo Dewy is ready. Start it with: run.bat
