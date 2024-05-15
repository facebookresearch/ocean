@REM Copyright (c) Meta Platforms, Inc. and affiliates.
@REM
@REM This source code is licensed under the MIT license found in the
@REM LICENSE file in the root directory of this source tree.

@echo off

@REM Determine the location of the source directory from the location of this script
set OCEAN_SOURCE_DIR=%~dp0..\..
set OCEAN_BUILD_ROOT_DIRECTORY=C:\tmp\ocean
set OCEAN_INSTALL_ROOT_DIRECTORY=C:\tmp\ocean

setlocal

set BUILD_FAILURES=

@REM Static, debug
set LIBRARY_TYPE=static
set BUILD_SHARED_LIBS=OFF
set BUILD_TYPE=Debug
call :run_build

@REM Static, release
set LIBRARY_TYPE=static
set BUILD_SHARED_LIBS=OFF
set BUILD_TYPE=Release
call :run_build

@REM Shared, debug
set LIBRARY_TYPE=shared
set BUILD_SHARED_LIBS=ON
set BUILD_TYPE=Debug
call :run_build

@REM Shared, release
set LIBRARY_TYPE=shared
set BUILD_SHARED_LIBS=ON
set BUILD_TYPE=Release
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

set OCEAN_BUILD_DIRECTORY=%OCEAN_BUILD_ROOT_DIRECTORY%\%LIBRARY_TYPE%_%BUILD_TYPE%
set OCEAN_INSTALL_DIRECTORY=%OCEAN_INSTALL_ROOT_DIRECTORY%\install\%LIBRARY_TYPE%_%BUILD_TYPE%
set OCEAN_THIRD_PARTY_DIRECTORY=%THIRD_PARTY_ROOT_DIRECTORY%\%LIBRARY_TYPE%_%BUILD_TYPE%

echo;
echo BUILD_TYPE: %BUILD_TYPE%
echo LIBRARY_TYPE: %LIBRARY_TYPE%
echo;
echo OCEAN_BUILD_DIRECTORY: %OCEAN_BUILD_DIRECTORY%
echo OCEAN_INSTALL_DIRECTORY: %OCEAN_INSTALL_DIRECTORY%
echo OCEAN_THIRD_PARTY_DIRECTORY: %OCEAN_THIRD_PARTY_DIRECTORY%

cmake -S %OCEAN_SOURCE_DIR% -B %OCEAN_BUILD_DIRECTORY% -DCMAKE_INSTALL_PREFIX=%OCEAN_INSTALL_DIRECTORY% -DCMAKE_CONFIGURATION_TYPES=%BUILD_TYPE% -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS%
cmake --build %OCEAN_BUILD_DIRECTORY% --config %BUILD_TYPE% --target install -- /m:16

echo;
echo;

if %errorlevel% neq 0 (
    set BUILD_FAILURES=%BUILD_FAILURES% %LIBRARY_TYPE%_%BUILD_TYPE%
    exit /b 1
) else (
    exit /b 0
)
