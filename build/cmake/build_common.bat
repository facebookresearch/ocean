@REM Copyright (c) Meta Platforms, Inc. and affiliates.
@REM
@REM This source code is licensed under the MIT license found in the
@REM LICENSE file in the root directory of this source tree.

@REM Common helper functions and dependency checks for Ocean build scripts.
@REM This script should be called by other build scripts, not executed directly.
@REM
@REM Usage: call "%~dp0build_common.bat" :check_build_dependencies

@echo off

if "%~1"==":check_build_dependencies" goto :check_build_dependencies
if "%~1"==":check_ocean_development_path" goto :check_ocean_development_path
if "%~1"==":check_cmake" goto :check_cmake

@REM If called without arguments, show usage
echo This script provides common build functions for Ocean.
echo It should be called from other build scripts, not run directly.
echo.
echo Available functions:
echo   :check_build_dependencies      - Check all required build tools
echo   :check_ocean_development_path  - Check if OCEAN_DEVELOPMENT_PATH is set
echo   :check_cmake                   - Check if CMake is installed
exit /b 0

:check_build_dependencies
call :check_ocean_development_path
if %errorlevel% neq 0 exit /b 1
call :check_cmake
if %errorlevel% neq 0 exit /b 1
exit /b 0

:check_ocean_development_path
if "%OCEAN_DEVELOPMENT_PATH%"=="" (
    echo ERROR: OCEAN_DEVELOPMENT_PATH is not set. 1>&2
    echo. 1>&2
    echo Please set OCEAN_DEVELOPMENT_PATH to the root of your Ocean development directory: 1>&2
    echo   set OCEAN_DEVELOPMENT_PATH=C:\path\to\ocean 1>&2
    exit /b 1
)
exit /b 0

:check_cmake
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo ERROR: CMake is not installed or not in your PATH. 1>&2
    echo. 1>&2
    echo Please install CMake before running this script: 1>&2
    echo   Download from: https://cmake.org/download/ 1>&2
    echo   Or use winget: winget install Kitware.CMake 1>&2
    echo   Or use choco:  choco install cmake 1>&2
    exit /b 1
)
exit /b 0
