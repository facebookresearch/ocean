@REM Copyright (c) Meta Platforms, Inc. and affiliates.
@REM
@REM This source code is licensed under the MIT license found in the
@REM LICENSE file in the root directory of this source tree.

echo off

setlocal

@REM Determine the location of the source directory from the location of this script
set OCEAN_THIRD_PARTY_SOURCE_DIR=%~dp0..\..\build\cmake\third-party

set BUILD_FAILURES=

@REM Static, debug
set BUILD_TYPE=Debug
set BUILD_SHARED_LIBS=OFF
set BUILD_DIRECTORY=C:\tmp\ocean\build\win\3P\static_%BUILD_TYPE%
set INSTALL_DIRECTORY=C:\tmp\ocean\install\win\static_%BUILD_TYPE%
call :run_build

@REM Static, release
set BUILD_TYPE=Release
set BUILD_SHARED_LIBS=OFF
set BUILD_DIRECTORY=C:\tmp\ocean\build\win\3P\static_%BUILD_TYPE%
set INSTALL_DIRECTORY=C:\tmp\ocean\install\win\static_%BUILD_TYPE%
call :run_build

@REM Shared, debug
set BUILD_TYPE=Debug
set BUILD_SHARED_LIBS=ON
set BUILD_DIRECTORY=C:\tmp\ocean\build\win\3P\shared_%BUILD_TYPE%
set INSTALL_DIRECTORY=C:\tmp\ocean\install\win\shared_%BUILD_TYPE%
call :run_build

@REM Shared, release
set BUILD_TYPE=Release
set BUILD_SHARED_LIBS=ON
set BUILD_DIRECTORY=C:\tmp\ocean\build\win\3P\shared_%BUILD_TYPE%
set INSTALL_DIRECTORY=C:\tmp\ocean\install\win\shared_%BUILD_TYPE%
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
call %OCEAN_THIRD_PARTY_SOURCE_DIR%\build_deps.bat windows %OCEAN_THIRD_PARTY_SOURCE_DIR% %BUILD_DIRECTORY% /m:16 "-DCMAKE_INSTALL_PREFIX=%INSTALL_DIRECTORY%" "-DCMAKE_CONFIGURATION_TYPES=%BUILD_TYPE%" "-DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS%"

@echo off
if %errorlevel% neq 0 (
    set BUILD_FAILURES=%BUILD_FAILURES% %LIBRARY_TYPE%_%BUILD_TYPE%_pass%BUILD_PASS%
    exit /b 1
) else (
    exit /b 0
)
