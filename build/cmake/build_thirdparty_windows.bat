@REM (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

echo off

setlocal

@REM Determine the location of the source directory from the location of this script
set OCEAN_THIRD_PARTY_SOURCE_DIR=%~dp0..\..\build\cmake\third-party

set BUILD_FAILURES=

@REM Static, debug
set BUILD_TYPE=Debug
set BUILD_SHARED_LIBS=OFF
set BUILD_DIRECTORY=C:\tmp\ocean\build\third-party\static_%BUILD_TYPE%
set INSTALL_DIRECTORY=C:\tmp\ocean\install\static_%BUILD_TYPE%
call :run_build

@REM Static, release
set BUILD_TYPE=Release
set BUILD_SHARED_LIBS=OFF
set BUILD_DIRECTORY=C:\tmp\ocean\build\third-party\static_%BUILD_TYPE%
set INSTALL_DIRECTORY=C:\tmp\ocean\install\static_%BUILD_TYPE%
call :run_build

@REM Shared, debug
set BUILD_TYPE=Debug
set BUILD_SHARED_LIBS=ON
set BUILD_DIRECTORY=C:\tmp\ocean\build\third-party\shared_%BUILD_TYPE%
set INSTALL_DIRECTORY=C:\tmp\ocean\install\shared_%BUILD_TYPE%
call :run_build

@REM Shared, release
set BUILD_TYPE=Release
set BUILD_SHARED_LIBS=ON
set BUILD_DIRECTORY=C:\tmp\ocean\build\third-party\shared_%BUILD_TYPE%
set INSTALL_DIRECTORY=C:\tmp\ocean\install\shared_%BUILD_TYPE%
call :run_build

if "%BUILD_FAILURES%" == "" (
    echo All builds were successful.
    exit /b 0
) else (
    echo [91mSome builds have failed.[0m 1>&2
    for %%f in (%BUILD_FAILURES%) do (
        echo [91m- %%f[0m 1>&2
    )
    exit /b 1
)

:run_build
set BUILD_PASS=0
call :build_pass

set BUILD_PASS=1
call :build_pass

set BUILD_PASS=2
call :build_pass

:build_pass
cmake -S %OCEAN_THIRD_PARTY_SOURCE_DIR% -B %BUILD_DIRECTORY% -DCMAKE_INSTALL_PREFIX=%INSTALL_DIRECTORY% -DCMAKE_CONFIGURATION_TYPES=%BUILD_TYPE% -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS% -DBUILD_PASS_INDEX=%BUILD_PASS%
cmake --build %BUILD_DIRECTORY% --config %BUILD_TYPE% --target install -- /m:16

@echo off
if %errorlevel% neq 0 (
    set BUILD_FAILURES=%BUILD_FAILURES% %LIBRARY_TYPE%_%BUILD_TYPE%_pass%BUILD_PASS%
    exit /b 1
) else (
    exit /b 0
)
