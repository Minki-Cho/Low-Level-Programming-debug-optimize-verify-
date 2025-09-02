@echo off
setlocal enabledelayedexpansion

REM Initialize parameters with default values
set "LIBRARY_TYPE=static"
set "BUILD_CONFIG=RelWithDebInfo"
set "BUILD_DIR=build"
set "FORCE_DBGHELP=OFF"

set PROFILER_STATIC_LIB="OFF"
set PROFILER_SHARED_LIB="OFF"

REM Check if no arguments are supplied
if "%~1"=="" (
    echo No arguments supplied
    call :print_help
    exit /b 0
)

REM Loop through arguments and assign to variables
:loop
if "%~1"=="" goto :end
set "arg=%~1"
if "!arg:~0,2!"=="--" set "arg=!arg:~2!"
if "!arg:~0,1!"=="-" set "arg=!arg:~1!"
if "!arg:~0,1!"=="/" set "arg=!arg:~1!"
if "!arg!"=="static" (
    set "LIBRARY_TYPE=static"
) else if "!arg!"=="dynamic" (
    set "LIBRARY_TYPE=dynamic"
) else if "!arg!"=="shared" (
    set "LIBRARY_TYPE=dynamic"
) else if "!arg!"=="forceDbgHelp" (
    set "FORCE_DBGHELP=ON"
) else if "!arg!"=="help" (
    call :print_help
    exit /b 0
) else if "!arg!"=="h" (
    call :print_help
    exit /b 0
) else (
    for /F "tokens=1,2 delims==" %%a in ("!arg!") do (
        if "%%a"=="lib" (
            set "LIBRARY_TYPE=%%b"
        ) else if "%%a"=="config" (
            set "BUILD_CONFIG=%%b"
        ) else if "%%a"=="dir" (
            set "BUILD_DIR=%%b"
        ) else if "%%a"=="forceDbgHelp" (
            set "FORCE_DBGHELP=ON"
        ) else (
            echo Invalid argument: %%a
            call :print_help
            exit /b 1
        )   
    )
)
shift
goto :loop

:end

IF "%LIBRARY_TYPE%"=="dynamic" (
    set PROFILER_STATIC_LIB="OFF"
    set PROFILER_SHARED_LIB="ON"
)
IF "%LIBRARY_TYPE%"=="shared" (
    set PROFILER_STATIC_LIB="OFF"
    set PROFILER_SHARED_LIB="ON"
)
IF "%LIBRARY_TYPE%"=="static" (
    set PROFILER_STATIC_LIB="ON"
    set PROFILER_SHARED_LIB="OFF"
)

echo cmake -G "Visual Studio 17 2022" -DPROFILER_BUILD_STATIC_LIBS=%PROFILER_STATIC_LIB% -DPROFILER_BUILD_SHARED_LIBS=%PROFILER_SHARED_LIB% -S ./liquidfun/Box2D -B ./%BUILD_DIR% -D FORCE_DBGHELP=%FORCE_DBGHELP%
cmake -G "Visual Studio 17 2022" -DPROFILER_BUILD_STATIC_LIBS=%PROFILER_STATIC_LIB% -DPROFILER_BUILD_SHARED_LIBS=%PROFILER_SHARED_LIB% -S ./liquidfun/Box2D -B ./%BUILD_DIR% -D FORCE_DBGHELP=%FORCE_DBGHELP%

echo cmake --build ./%BUILD_DIR% --config %BUILD_CONFIG%
cmake --build ./%BUILD_DIR% --config %BUILD_CONFIG%

exit /b 0

REM Function to print the help message
:print_help
echo Usage: %0 [options]
echo.
echo Options:
echo   --lib=static^|dynamic^|shared    Set the library type (default: static)
echo   --config=CONFIG                  Set the build configuration (default: RelWithDebInfo)
echo   --dir=DIR                        Set the build directory (default: build)
echo   --forceDbgHelp                   Force debug help (default: OFF)
echo   --static                         Set library type to static
echo   --dynamic                        Set library type to dynamic
echo   --shared                         Set library type to shared
echo   --help, -h                       Print this help message
exit /b 0
