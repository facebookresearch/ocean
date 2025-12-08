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
:loop
if not "%~1"=="" (
    if "%~1"=="-h" (
        set "-h=1"
        shift /1
        goto :loop
    )
    if "%~1"=="-i" (
        if "%~2"=="" (
            echo Error: -i requires an argument
            set "-h=1"
            goto :endloop
        )
        set "-i=%~2"
        shift /1
        shift /1
        goto :loop
    )
    if "%~1"=="-o" (
        if "%~2"=="" (
            echo Error: -o requires an argument
            set "-h=1"
            goto :endloop
        )
        set "-o=%~2"
        shift /1
        shift /1
        goto :loop
    )
    if "%~1"=="-v" (
        if "%~2"=="" (
            echo Error: -v requires an argument
            set "-h=1"
            goto :endloop
        )
        set "-v=%~2"
        shift /1
        shift /1
        goto :loop
    )
    echo Error: Invalid option %~1
    set "-h=1"
    shift /1
    goto :loop
)
:endloop

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
@REM Naming convention: {platform}_{arch}_{compiler}{version}_{linktype}[_debug]
@REM
@REM Examples:
@REM   - win_x64_vc143_static_debug (Windows x64, MSVC 14.3, static, debug)
@REM   - win_x64_vc143_static (Windows x64, MSVC 14.3, static)
@REM   - android_arm64_cl18_static (Android ARM64, Clang 18, static)
@REM   - ios_arm64_xc15_static (iOS ARM64, Xcode 15, static)
@REM
@REM Input formats:
@REM   - 3 components: {platform}_{linktype}_{config} (windows, macos, linux)
@REM   - 4 components: {platform}_{variant}_{linktype}_{config} (android ABI, iOS toolchain)
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
        call :build_platform_name_android "!_p2!" "!_p3!" "!_p4!" !_outvar!
        exit /b 0
    )
    if "!_p1!"=="ios" (
        call :build_platform_name_ios "!_p2!" "!_p3!" "!_p4!" !_outvar!
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
        call :build_platform_name_windows "!_p2!" "!_p3!" !_outvar!
        exit /b 0
    )
    if "!_p1!"=="macos" (
        call :build_platform_name_macos "!_p2!" "!_p3!" !_outvar!
        exit /b 0
    )
    if "!_p1!"=="linux" (
        call :build_platform_name_linux "!_p2!" "!_p3!" !_outvar!
        exit /b 0
    )
)

exit /b 0

:build_platform_name_windows
@REM Windows 3-component: %1=linktype, %2=config, %3=outvar
@REM Output format: win_x64_vc143_{linktype}[_debug]
set "_linktype=%~1"
set "_config=%~2"
set "_outvar=%~3"

@REM Default to x64 and VS2022 (vc143)
set "_arch=x64"
set "_compiler=vc143"

if /I "!_config!"=="Debug" (
    set "!_outvar!=win_!_arch!_!_compiler!_!_linktype!_debug"
) else if /I "!_config!"=="Release" (
    set "!_outvar!=win_!_arch!_!_compiler!_!_linktype!"
)
exit /b 0

:build_platform_name_macos
@REM macOS 3-component: %1=linktype, %2=config, %3=outvar
@REM Output format: osx_{arch}_xc15_{linktype}[_debug]
set "_linktype=%~1"
set "_config=%~2"
set "_outvar=%~3"

@REM Default to x64 and Xcode 15
set "_arch=x64"
set "_compiler=xc15"

if /I "!_config!"=="Debug" (
    set "!_outvar!=osx_!_arch!_!_compiler!_!_linktype!_debug"
) else if /I "!_config!"=="Release" (
    set "!_outvar!=osx_!_arch!_!_compiler!_!_linktype!"
)
exit /b 0

:build_platform_name_linux
@REM Linux 3-component: %1=linktype, %2=config, %3=outvar
@REM Output format: linux_{arch}_gcc13_{linktype}[_debug]
set "_linktype=%~1"
set "_config=%~2"
set "_outvar=%~3"

@REM Default to x64 and GCC 13
set "_arch=x64"
set "_compiler=gcc13"

if /I "!_config!"=="Debug" (
    set "!_outvar!=linux_!_arch!_!_compiler!_!_linktype!_debug"
) else if /I "!_config!"=="Release" (
    set "!_outvar!=linux_!_arch!_!_compiler!_!_linktype!"
)
exit /b 0

:build_platform_name_android
@REM Android 4-component: %1=variant(ABI), %2=linktype, %3=config, %4=outvar
@REM Output format: android_{arch}_cl18_{linktype}[_debug]
set "_abi=%~1"
set "_linktype=%~2"
set "_config=%~3"
set "_outvar=%~4"

@REM Map ABI to architecture
call :map_android_abi "!_abi!" _arch

@REM Default to Clang 18 (NDK r26+)
set "_compiler=cl18"

if /I "!_config!"=="Debug" (
    set "!_outvar!=android_!_arch!_!_compiler!_!_linktype!_debug"
) else if /I "!_config!"=="Release" (
    set "!_outvar!=android_!_arch!_!_compiler!_!_linktype!"
)
exit /b 0

:map_android_abi
@REM Map Android ABI to architecture name
set "_abi=%~1"
set "_outvar=%~2"

if "!_abi!"=="arm64-v8a" (
    set "!_outvar!=arm64"
) else if "!_abi!"=="armeabi-v7a" (
    set "!_outvar!=arm"
) else if "!_abi!"=="x86_64" (
    set "!_outvar!=x64"
) else if "!_abi!"=="x86" (
    set "!_outvar!=x86"
) else (
    set "!_outvar!=!_abi!"
)
exit /b 0

:build_platform_name_ios
@REM iOS 4-component: %1=variant(toolchain), %2=linktype, %3=config, %4=outvar
@REM Output format: ios_{arch}_xc15_{linktype}[_debug]
set "_toolchain=%~1"
set "_linktype=%~2"
set "_config=%~3"
set "_outvar=%~4"

@REM Map toolchain to architecture
call :map_ios_toolchain "!_toolchain!" _arch

@REM Default to Xcode 15
set "_compiler=xc15"

if /I "!_config!"=="Debug" (
    set "!_outvar!=ios_!_arch!_!_compiler!_!_linktype!_debug"
) else if /I "!_config!"=="Release" (
    set "!_outvar!=ios_!_arch!_!_compiler!_!_linktype!"
)
exit /b 0

:map_ios_toolchain
@REM Map iOS toolchain to architecture name
set "_toolchain=%~1"
set "_outvar=%~2"

if "!_toolchain!"=="OS64" (
    set "!_outvar!=arm64"
) else if "!_toolchain!"=="SIMULATOR64" (
    set "!_outvar!=x64"
) else if "!_toolchain!"=="SIMULATORARM64" (
    set "!_outvar!=arm64"
) else if "!_toolchain!"=="OS" (
    set "!_outvar!=arm"
) else (
    set "!_outvar!=!_toolchain!"
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
