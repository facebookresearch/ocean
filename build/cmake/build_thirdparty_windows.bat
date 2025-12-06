@REM Copyright (c) Meta Platforms, Inc. and affiliates.
@REM
@REM This source code is licensed under the MIT license found in the
@REM LICENSE file in the root directory of this source tree.

@echo off

@REM Determine the location of the source directory from the location of this script
set "OCEAN_THIRD_PARTY_SOURCE_DIR=%~dp0..\..\build\cmake\third-party"
set "DEFAULT_BUILD_PATH=%cd%\ocean_build_thirdparty"
set "DEFAULT_INSTALL_PATH=%cd%\ocean_install_thirdparty"
set "OCEAN_PLATFORM=windows"

setlocal enableDelayedExpansion

set "options=-install:%DEFAULT_INSTALL_PATH% -build:%DEFAULT_BUILD_PATH% -config:debug,release -link:static,shared -archive:NULL -subdivide:OFF -h:"
set "SHOW_HELP=0"

for %%O in (%options%) do for /f "tokens=1,* delims=:" %%A in ("%%O") do set "%%A=%%~B"
:loop
if not "%~1"=="" (
  set "test=!options:*%~1:=! "
  if "!test!"=="!options! " (
      echo Error: Invalid option %~1
      set SHOW_HELP=1
  ) else if "!test:~0,1!"==" " (
      set "%~1=1"
  ) else (
      setlocal disableDelayedExpansion
      if "%~2"=="" (
        echo [91mRequired argument for %~1 missing[0m 1>&2
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
    echo   -h : This summary
    echo(
    endlocal
    exit /b

:skip_help

set BUILD_FAILURES=

for %%c in (!-config!) do (

  if /I %%c==debug (
    set BUILD_TYPE=Debug
  ) else if /I %%c==release (
    set BUILD_TYPE=Release
  ) else (
    echo Invalid build config %%c
    exit /b
  )

  for %%l in (!-link!) do (
    if /I %%l==static (
      set BUILD_SHARED_LIBS=OFF
      set bibase=%OCEAN_PLATFORM%_static_!BUILD_TYPE!
    ) else if /I %%l==shared (
      set BUILD_SHARED_LIBS=ON
      set bibase=%OCEAN_PLATFORM%_shared_!BUILD_TYPE!
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
call "%OCEAN_THIRD_PARTY_SOURCE_DIR%\build_deps.bat" windows "%OCEAN_THIRD_PARTY_SOURCE_DIR%" !BUILD_DIRECTORY! /m:16 !-subdivide! "-DCMAKE_INSTALL_PREFIX=!INSTALL_DIRECTORY!" "-DCMAKE_CONFIGURATION_TYPES=!BUILD_TYPE!" "-DBUILD_SHARED_LIBS=!BUILD_SHARED_LIBS!" "-DCMAKE_FIND_ROOT_PATH=!INSTALL_DIRECTORY!" "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"

@echo off
if %errorlevel% neq 0 (
    set BUILD_FAILURES=%BUILD_FAILURES% !bibase!
    exit /b 1
) else (
    exit /b 0
)
