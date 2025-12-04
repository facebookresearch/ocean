@REM Copyright (c) Meta Platforms, Inc. and affiliates.
@REM
@REM This source code is licensed under the MIT license found in the
@REM LICENSE file in the root directory of this source tree.

@REM Script to reorganize third-party library installations from CMake --subdivide
@REM structure to a version-first, platform-organized structure.
@REM
@REM Supported platforms: Windows, Android (built on Windows)
@REM
@REM Input structure (--subdivide output):
@REM   install_3p\
@REM   +-- windows_static_Debug\
@REM   |   +-- zlib\
@REM   |   |   +-- include\
@REM   |   |   +-- lib\
@REM   |   +-- eigen\
@REM   |       +-- include\
@REM   +-- android_arm64-v8a_static_Debug\
@REM       +-- zlib\
@REM           +-- include\
@REM           +-- lib\
@REM
@REM Output structure (reorganized):
@REM   reorganized_3p\
@REM   +-- zlib\
@REM   |   +-- 1.x.x\
@REM   |       +-- include\
@REM   |       +-- lib\
@REM   |           +-- windows_static_debug\
@REM   |           +-- android_arm64-v8a_static_debug\
@REM   +-- eigen\
@REM       +-- 3.x.x\
@REM           +-- include\

@echo off
setlocal EnableDelayedExpansion

set SCRIPT_DIR=%~dp0
set INPUT_DIR=
set OUTPUT_DIR=%cd%\reorganized_3p
set VERSION_MAP_FILE=%SCRIPT_DIR%third-party\library_versions.txt

@REM Parse command line arguments
set "options=-i: -o:"%OUTPUT_DIR%" -v:"%VERSION_MAP_FILE%" -h:"

for %%O in (%options%) do for /f "tokens=1,* delims=:" %%A in ("%%O") do set "%%A=%%~B"
:loop
if not "%~1"=="" (
    set "test=!options:*%~1:=! "
    if "!test!"=="!options! " (
        echo Error: Invalid option %~1
        set -h=1
    ) else if "!test:~0,1!"==" " (
        set "%~1=1"
    ) else (
        set "%~1=%~2"
        shift /1
    )
    shift /1
    goto :loop
)

if "!-h!"=="1" (
    echo Script to reorganize third-party library installations:
    echo.
    echo Supported platforms: Windows, Android ^(built on Windows^)
    echo.
    echo   %~n0 [-h] -i INPUT_DIR [-o OUTPUT_DIR] [-v VERSION_MAP]
    echo.
    echo Arguments:
    echo.
    echo   -i INPUT_DIR : The directory containing --subdivide output
    echo                  ^(e.g., install_3p with subdirectories like windows_static_Debug^)
    echo.
    echo   -o OUTPUT_DIR : The output directory for reorganized structure.
    echo                   Default: %OUTPUT_DIR%
    echo.
    echo   -v VERSION_MAP : Optional file mapping library names to versions.
    echo                    Default: %VERSION_MAP_FILE%
    echo.
    echo   -h : This summary
    echo.
    exit /b 0
)

@REM Validate required arguments
if "!-i!"=="" (
    echo ERROR: Input directory ^(-i^) is required
    echo.
    echo Usage: %~n0 -i INPUT_DIR [-o OUTPUT_DIR]
    exit /b 1
)

set INPUT_DIR=!-i!
set OUTPUT_DIR=!-o!
set VERSION_MAP_FILE=!-v!

@REM Validate input directory exists
if not exist "!INPUT_DIR!" (
    echo ERROR: Input directory does not exist: !INPUT_DIR!
    exit /b 1
)

echo Reorganizing third-party libraries...
echo.
echo Input directory: !INPUT_DIR!
echo Output directory: !OUTPUT_DIR!
echo.

@REM Create output directory
if not exist "!OUTPUT_DIR!" mkdir "!OUTPUT_DIR!"

@REM Track processed headers (simplified - no header deduplication in Windows version)
set PROCESSED_HEADERS=

@REM Process each platform/config directory
for /d %%P in ("!INPUT_DIR!\*") do (
    set "PLATFORM_DIR=%%P"
    set "PLATFORM_BASENAME=%%~nxP"

    echo Processing platform: !PLATFORM_BASENAME!

    @REM Map platform name
    call :map_platform_name "!PLATFORM_BASENAME!" MAPPED_PLATFORM
    if "!MAPPED_PLATFORM!"=="" (
        echo WARNING: Skipping invalid platform directory: !PLATFORM_BASENAME!
    ) else (
        echo   Mapped to: !MAPPED_PLATFORM!

        @REM Process each library in this platform directory
        for /d %%L in ("!PLATFORM_DIR!\*") do (
            set "LIB_DIR=%%L"
            set "LIB_NAME=%%~nxL"

            echo   Processing library: !LIB_NAME!

            @REM Get version
            call :get_library_version "!LIB_DIR!" "!LIB_NAME!" LIB_VERSION
            echo     Version: !LIB_VERSION!

            @REM Create output structure
            set "OUTPUT_LIB_DIR=!OUTPUT_DIR!\!LIB_NAME!\!LIB_VERSION!"
            if not exist "!OUTPUT_LIB_DIR!" mkdir "!OUTPUT_LIB_DIR!"

            @REM Copy headers (only if not already copied for this library)
            echo !PROCESSED_HEADERS! | findstr /C:"[!LIB_NAME!]" >nul
            if errorlevel 1 (
                if exist "!LIB_DIR!\include" (
                    echo     Copying headers...
                    if not exist "!OUTPUT_LIB_DIR!\include" mkdir "!OUTPUT_LIB_DIR!\include"
                    robocopy "!LIB_DIR!\include" "!OUTPUT_LIB_DIR!\include" /E /XF *.cmake /XD cmake pkgconfig >nul
                    set "PROCESSED_HEADERS=!PROCESSED_HEADERS![!LIB_NAME!]"
                )
            ) else (
                echo     Headers already copied
            )

            @REM Copy libraries to platform-specific subdirectory
            if exist "!LIB_DIR!\lib" (
                echo     Copying libraries to lib\!MAPPED_PLATFORM!\...
                set "OUTPUT_PLATFORM_LIB=!OUTPUT_LIB_DIR!\lib\!MAPPED_PLATFORM!"
                if not exist "!OUTPUT_PLATFORM_LIB!" mkdir "!OUTPUT_PLATFORM_LIB!"
                robocopy "!LIB_DIR!\lib" "!OUTPUT_PLATFORM_LIB!" /E /XF *.cmake *.pc /XD cmake pkgconfig >nul
            )
        )
    )
    echo.
)

echo Reorganization complete!
echo.
echo Output directory: !OUTPUT_DIR!
exit /b 0

@REM ============================================================================
@REM Function: map_platform_name
@REM Maps CMake platform/config names to Ocean-native platform names
@REM
@REM Input: %1 = platform directory name (e.g., windows_static_Debug)
@REM Output: Sets variable named by %2 to mapped name
@REM ============================================================================
:map_platform_name
set "_input=%~1"
set "_outvar=%~2"
set "!_outvar!="

@REM Try 4-component match first (android, ios)
@REM Format: {platform}_{variant}_{linktype}_{config}
for /f "tokens=1,2,3,4 delims=_" %%A in ("!_input!") do (
    set "_p1=%%A"
    set "_p2=%%B"
    set "_p3=%%C"
    set "_p4=%%D"
)

@REM Check if we have 4 components
if not "!_p4!"=="" (
    @REM Validate platform for 4-component names
    if "!_p1!"=="android" (
        call :build_platform_name "!_p1!" "!_p2!" "!_p3!" "!_p4!" !_outvar!
        exit /b 0
    )
    if "!_p1!"=="ios" (
        call :build_platform_name "!_p1!" "!_p2!" "!_p3!" "!_p4!" !_outvar!
        exit /b 0
    )
)

@REM Try 3-component match (windows, macos, linux)
for /f "tokens=1,2,3 delims=_" %%A in ("!_input!") do (
    set "_p1=%%A"
    set "_p2=%%B"
    set "_p3=%%C"
)

@REM Check if we have exactly 3 components (no 4th)
if "!_p4!"=="" if not "!_p3!"=="" (
    @REM Validate platform for 3-component names
    if "!_p1!"=="windows" (
        call :build_platform_name_3 "!_p1!" "!_p2!" "!_p3!" !_outvar!
        exit /b 0
    )
    if "!_p1!"=="macos" (
        call :build_platform_name_3 "!_p1!" "!_p2!" "!_p3!" !_outvar!
        exit /b 0
    )
    if "!_p1!"=="linux" (
        call :build_platform_name_3 "!_p1!" "!_p2!" "!_p3!" !_outvar!
        exit /b 0
    )
)

exit /b 0

:build_platform_name
@REM 4-component: %1=platform, %2=variant, %3=linktype, %4=config, %5=outvar
set "_platform=%~1"
set "_variant=%~2"
set "_linktype=%~3"
set "_config=%~4"
set "_outvar=%~5"

if /I "!_config!"=="Debug" (
    set "!_outvar!=!_platform!_!_variant!_!_linktype!_debug"
) else if /I "!_config!"=="Release" (
    set "!_outvar!=!_platform!_!_variant!_!_linktype!"
)
exit /b 0

:build_platform_name_3
@REM 3-component: %1=platform, %2=linktype, %3=config, %4=outvar
set "_platform=%~1"
set "_linktype=%~2"
set "_config=%~3"
set "_outvar=%~4"

if /I "!_config!"=="Debug" (
    set "!_outvar!=!_platform!_!_linktype!_debug"
) else if /I "!_config!"=="Release" (
    set "!_outvar!=!_platform!_!_linktype!"
)
exit /b 0

@REM ============================================================================
@REM Function: get_library_version
@REM Gets library version from version.txt or defaults to "current"
@REM
@REM Input: %1 = library directory, %2 = library name
@REM Output: Sets variable named by %3 to version string
@REM ============================================================================
:get_library_version
set "_lib_dir=%~1"
set "_lib_name=%~2"
set "_outvar=%~3"
set "!_outvar!=current"

@REM First priority: Check for version.txt in the library directory
if exist "!_lib_dir!\version.txt" (
    set /p _version=<"!_lib_dir!\version.txt"
    if not "!_version!"=="" (
        @REM Trim whitespace
        for /f "tokens=* delims= " %%a in ("!_version!") do set "_version=%%a"
        set "!_outvar!=!_version!"
        exit /b 0
    )
)

@REM Second priority: Try version map file if it exists
if exist "!VERSION_MAP_FILE!" (
    for /f "tokens=1,2 delims==" %%A in (!VERSION_MAP_FILE!) do (
        if "%%A"=="!_lib_name!" (
            set "!_outvar!=%%B"
            exit /b 0
        )
    )
)

exit /b 0
