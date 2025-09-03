@echo off
setlocal enabledelayedexpansion

REM Initialize parameters with default values
set "BUILD_CONFIG=Debug"
set "BUILD_DIR=build"
set "FORCE_DBGHELP=OFF"

REM Loop through arguments and assign to variables
:loop
if "%~1"=="" goto :end
for /F "tokens=1,2 delims==" %%a in ("%~1") do (
    if "%%a"=="BUILD_CONFIG" set "BUILD_CONFIG=%%b"
    if "%%a"=="BUILD_DIR" set "BUILD_DIR=%%b"
    if "%%a"=="FORCE_DBGHELP" set "FORCE_DBGHELP=ON"
)
shift
goto :loop

:end

echo cmake -G "Visual Studio 17 2022" -A x64 -S . -B ./%BUILD_DIR% -D FORCE_DBGHELP=%FORCE_DBGHELP%
cmake -G "Visual Studio 17 2022" -A x64 -S . -B ./%BUILD_DIR% -D FORCE_DBGHELP=%FORCE_DBGHELP%

REM echo cmake --build %BUILD_DIR% --config %BUILD_CONFIG%
REM make --build %BUILD_DIR% --config %BUILD_CONFIG%
