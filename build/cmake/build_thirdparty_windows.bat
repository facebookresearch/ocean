@REM Copyright (c) Meta Platforms, Inc. and affiliates.
@REM
@REM This source code is licensed under the MIT license found in the
@REM LICENSE file in the root directory of this source tree.

@echo off

@REM Check for required dependencies
call "%~dp0build_common.bat" :check_build_dependencies
if %errorlevel% neq 0 exit /b 1

@REM Determine the location of the source directory from the location of this script
set "OCEAN_THIRD_PARTY_SOURCE_DIR=%~dp0..\..\build\cmake\third-party"
set "DEFAULT_BUILD_PATH=%cd%\ocean_build_thirdparty"
set "DEFAULT_INSTALL_PATH=%cd%\ocean_install_thirdparty"
set "OCEAN_PLATFORM=windows"

@REM Detect architecture and map to folder naming
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    set "OCEAN_ARCH=x64"
) else if "%PROCESSOR_ARCHITECTURE%"=="x86" (
    set "OCEAN_ARCH=x86"
) else if "%PROCESSOR_ARCHITECTURE%"=="ARM64" (
    set "OCEAN_ARCH=arm64"
) else (
    set "OCEAN_ARCH=%PROCESSOR_ARCHITECTURE%"
)

setlocal enableDelayedExpansion

set "options=-install:%DEFAULT_INSTALL_PATH% -build:%DEFAULT_BUILD_PATH% -config:debug -link:static -archive:NULL -subdivide:OFF -log-level:ERROR -generator: -h:"

set "SHOW_HELP=0"

for %%O in (%options%) do for /f "tokens=1,* delims=:" %%A in ("%%O") do set "%%A=%%~B"

@REM Override with multi-value defaults (must be after options parsing above)
set "-config=debug release"
@REM Default to static only; user can specify "-link shared" or "-link "static shared"" for shared builds

:loop
if not "%~1"=="" (
  @REM Handle -option=value syntax (for values with spaces like -generator="Visual Studio 17 2022")
  set "arg=%~1"
  set "opt="
  set "optval="
  for /f "tokens=1,* delims==" %%A in ("!arg!") do (
    set "opt=%%A"
    set "optval=%%B"
  )
  if not "!optval!"=="" (
    @REM This is -option=value syntax - use call to handle nested variable expansion
    call set "test=%%options:*!opt!:=%% "
    if "!test!"=="!options! " (
      echo Error: Invalid option !opt!
      set SHOW_HELP=1
    ) else (
      set "!opt!=!optval!"
    )
    shift /1
    goto :loop
  )

  @REM Standard -option value syntax
  set "test=!options:*%~1:=! "
  if "!test!"=="!options! " (
      echo Error: Invalid option %~1
      set SHOW_HELP=1
  ) else if "!test:~0,1!"==" " (
      set "%~1=1"
  ) else (
      setlocal disableDelayedExpansion
      if "%~2"=="" (
        echo [91mRequired argument for %~1 missing[0m 1>&2
        exit /b 99
      )
      set "val=%~2"
      call :escapeVal
      setlocal enableDelayedExpansion
      for /f delims^=^ eol^= %%A in ("!val!") do endlocal&endlocal&set "%~1=%%A"
      shift /1
  )
  shift /1
  goto :loop
)
goto :endArgs
:escapeVal
set "val=%val:^=^^%"
set "val=%val:!=^!%"
exit /b
:endArgs

if "!SHOW_HELP!"=="1" goto :show_help
goto :skip_help

:show_help
    setlocal disableDelayedExpansion
    echo Script to build the third-party libraries required by Ocean :
    echo(
    echo  %~n0  [-h] [-install INSTALL_DIR] [-build BUILD_DIR] [-config BUILD_CONFIG]
    echo                    [-link LINKING_TYPE] [-archive ARCHIVE] [-subdivide ON/OFF]
    echo(
    echo Arguments:
    echo(
    echo   List arguments should be space-delimited in double quotes
    echo(
    echo   IMPORTANT: For options with values containing spaces, use the equals sign syntax:
    echo              -generator="Visual Studio 17 2022"
    echo(
    echo   -install INSTALL_DIR : The optional location where the third-party libraries of Ocean will
    echo                 be installed. Otherwise builds will be installed to: !-install!
    echo(
    echo   -build BUILD_DIR : The optional location where the third-party libraries of Ocean will
    echo                 be built. Otherwise builds will be installed to: !-build!
    echo(
    echo   -config BUILD_CONFIG : The optional build configs to be built; valid values are:
    echo                 Multiple values must be separated by spaces (inside double quotes^).
    echo                 Default value is nothing is specified: !-config!
    echo(
    echo   -link LINKING_TYPE : The optional linking type for which will be built; valid values are:
    echo                 Multiple values must be separated by spaces (inside double quotes^).
    echo                 Default value if nothing is specified: !-link!
    echo(
    echo   -archive ARCHIVE : If specified, this will copy the contents of INSTALL_DIR after the build
    echo                 into a ZIP archive; the path to this archive must exist.
    echo(
    echo   -subdivide ON/OFF : Install each library into its own subdirectory. When enabled,
    echo                 libraries will be installed to {INSTALL_DIR}\library_name\{lib,include,...}.
    echo                 Default: OFF (flat structure for backward compatibility)
    echo(
    echo   -log-level LEVEL : Set the CMake log level. Valid values are:
    echo                 ERROR, WARNING, NOTICE, STATUS, VERBOSE, DEBUG, TRACE
    echo                 Default: ERROR (only show errors^)
    echo(
    echo   -generator GEN : CMake generator to use. For Windows, you can specify a
    echo                 Visual Studio version. Use equals sign syntax for values with spaces:
    echo                 -generator="Visual Studio 17 2022"   (VS 2022)
    echo                 -generator="Visual Studio 16 2019"   (VS 2019)
    echo                 By default, CMake auto-detects the newest available version.
    echo(
    echo   -h : This summary
    echo(
    endlocal
    exit /b

:skip_help

@REM Detect Visual Studio toolset version AFTER argument parsing
@REM If -generator is specified, extract version from it; otherwise use vswhere
call :detect_vs_toolset "!-generator!"
if "!OCEAN_VS_TOOLSET!"=="" (
    echo Warning: Could not detect Visual Studio toolset version, using default naming.
    set "OCEAN_VS_TOOLSET="
) else (
    set "OCEAN_VS_TOOLSET=_!OCEAN_VS_TOOLSET!"
)

set BUILD_FAILURES=

for %%c in (!-config!) do (

  if /I %%c==debug (
    set BUILD_TYPE=Debug
    set BUILD_TYPE_LOWER=debug
  ) else if /I %%c==release (
    set BUILD_TYPE=Release
    set BUILD_TYPE_LOWER=release
  ) else (
    echo Invalid build config %%c
    exit /b
  )

  for %%l in (!-link!) do (
    if /I %%l==static (
      set BUILD_SHARED_LIBS=OFF
      set bibase=%OCEAN_PLATFORM%\%OCEAN_ARCH%%OCEAN_VS_TOOLSET%_static_!BUILD_TYPE_LOWER!
    ) else if /I %%l==shared (
      set BUILD_SHARED_LIBS=ON
      set bibase=%OCEAN_PLATFORM%\%OCEAN_ARCH%%OCEAN_VS_TOOLSET%_shared_!BUILD_TYPE_LOWER!
    ) else (
      echo Invalid link mode %%l
      exit /b
    )

    set BUILD_DIRECTORY="!-build!\!bibase!"
    set INSTALL_DIRECTORY=!-install!\!bibase!

    echo BUILD_TYPE           !BUILD_TYPE!
    echo BUILD_SHARED_LIBS    !BUILD_SHARED_LIBS!
    echo BUILD_DIRECTORY      !BUILD_DIRECTORY!
    echo INSTALL_DIRECTORY    !INSTALL_DIRECTORY!

    call :run_build
  )
)

if "%BUILD_FAILURES%" == "" (
    echo All builds were successful.

    if NOT !-archive! == NULL (
      echo Creating !-archive!
      pushd !-install! && (tar -a -c -f !-archive! . & popd)
      echo done
    )

    exit /b 0

) else (
    echo [91mSome builds have failed.[0m 1>&2
    for %%f in (%BUILD_FAILURES%) do (
        echo [91m- %%f[0m 1>&2
    )
    exit /b 1
)

:run_build
@REM Set quiet flag for MSBuild when log level is ERROR
set MSBUILD_VERBOSITY=
if /I "!-log-level!"=="ERROR" set MSBUILD_VERBOSITY=/v:q

call "%OCEAN_THIRD_PARTY_SOURCE_DIR%\build_deps.bat" windows "%OCEAN_THIRD_PARTY_SOURCE_DIR%" !BUILD_DIRECTORY! "/m:16 !MSBUILD_VERBOSITY!" !-subdivide! !-log-level! "!-generator!" "-DCMAKE_INSTALL_PREFIX=!INSTALL_DIRECTORY!" "-DCMAKE_CONFIGURATION_TYPES=!BUILD_TYPE!" "-DBUILD_SHARED_LIBS=!BUILD_SHARED_LIBS!" "-DCMAKE_FIND_ROOT_PATH=!INSTALL_DIRECTORY!" "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"

@echo off
if %errorlevel% neq 0 (
    set BUILD_FAILURES=%BUILD_FAILURES% !bibase!
    exit /b 1
) else (
    exit /b 0
)

:detect_vs_toolset
@REM Detect Visual Studio toolset version
@REM If generator is specified (e.g., "Visual Studio 17 2022"), extract version from it
@REM Otherwise use vswhere to detect the latest installed version
@REM Returns OCEAN_VS_TOOLSET as vc142, vc143, vc145, etc.

set "OCEAN_VS_TOOLSET="
set "GENERATOR_ARG=%~1"

@REM If generator is specified, try to extract VS version from it
if not "!GENERATOR_ARG!"=="" (
    @REM Extract the VS major version number from "Visual Studio XX YYYY"
    @REM The format is: "Visual Studio <major_version> <year>"
    for /f "tokens=3" %%v in ("!GENERATOR_ARG!") do (
        set "VS_MAJOR_VERSION=%%v"
    )
    if defined VS_MAJOR_VERSION (
        @REM Map VS major version to toolset version
        if !VS_MAJOR_VERSION! GEQ 18 (
            set /a "TOOLSET_NUM=VS_MAJOR_VERSION + 127"
        ) else (
            set /a "TOOLSET_NUM=VS_MAJOR_VERSION + 126"
        )
        set "OCEAN_VS_TOOLSET=vc!TOOLSET_NUM!"
        exit /b 0
    )
)

@REM Fall back to vswhere detection if no generator specified or extraction failed
@REM Try to find vswhere
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    set "VSWHERE=%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe"
)
if not exist "%VSWHERE%" (
    exit /b 1
)

@REM Get the Visual Studio major version (16=2019, 17=2022, 18=2026)
for /f "usebackq tokens=1 delims=." %%v in (`"%VSWHERE%" -latest -property installationVersion`) do (
    set "VS_MAJOR_VERSION=%%v"
)

if "%VS_MAJOR_VERSION%"=="" (
    exit /b 1
)

@REM Map VS major version to toolset version
@REM VS 2015 (14.x) = vc140, VS 2017 (15.x) = vc141, VS 2019 (16.x) = vc142
@REM VS 2022 (17.x) = vc143, VS 2026 (18.x) = vc145
if %VS_MAJOR_VERSION% GEQ 18 (
    set /a "TOOLSET_NUM=VS_MAJOR_VERSION + 127"
) else (
    set /a "TOOLSET_NUM=VS_MAJOR_VERSION + 126"
)
set "OCEAN_VS_TOOLSET=vc%TOOLSET_NUM%"

exit /b 0
