@REM Copyright (c) Meta Platforms, Inc. and affiliates.
@REM
@REM This source code is licensed under the MIT license found in the
@REM LICENSE file in the root directory of this source tree.

echo off

@REM Check for required dependencies
call "%~dp0build_common.bat" :check_build_dependencies
if %errorlevel% neq 0 exit /b 1

@REM Determine the location of the source directory from the location of this script
set OCEAN_THIRD_PARTY_SOURCE_DIR=%~dp0..\..\build\cmake\third-party
set OCEAN_SOURCE_DIR=%~dp0..\..

set OCEAN_PLATFORM=android

if "%ANDROID_HOME%" == "" (
    echo "ERROR: Set ANDROID_HOME to the location of your Android SDK installation."
    exit /b 1
)

if "%ANDROID_NDK%" == "" (
    echo "ERROR: Set ANDROID_NDK to the location of your Android NDK installation."
    exit /b 1
)

if "%JAVA_HOME%" == "" (
    echo "ERROR: Set JAVA_HOME to the location of your Java installation."
    exit /b 1
)

set ANDROID_SDK_VERSION=android-34

@echo off

setlocal enableDelayedExpansion

set "options=-android_abi:"arm64-v8a" -install:"%cd%\ocean_install" -build:"%cd%\ocean_build" -config:"debug release" -link:"static" -third-party:"%cd%\ocean_install_thirdparty" -archive:NULL -h: -sdk:%ANDROID_SDK_VERSION% -quest:"

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
      setlocal disableDelayedExpansion
      if "%~2"=="" (
        echo [91mRequired argument for %~1 missing[0m 1>&2
        exit /b 99
      )
      set "val=%~2"
      call :escapeVal
      setlocal enableDelayedExpansion
      for /f delims^=^ eol^= %%A in ("!val!") do endlocal&endlocal&set "%~1=%%A" !
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

if !-h!==1 (
    echo Script to build Ocean for %OCEAN_PLATFORM% :
    echo(
    echo  %~n0  [-h] [-android_abi ABI_LIST] [-install INSTALL_DIR] [-build BUILD_DIR]
    echo                    [-config BUILD_CONFIG] [-link LINKING_TYPE] [-archive ARCHIVE]
    echo(
    echo Arguments:
    echo(
    echo   List arguments should be space-delimited in double quotes
    echo(
    echo   -android_abi ABI_LIST : A list of Android ABI's as build target platforms.
    echo                 Default value: !-android_abi!
    echo                 Valid values:  arm64-v8a armeabi-v7a x86_64 x86
    echo(
    echo   -install INSTALL_DIR : The optional location where the third-party libraries of Ocean will
    echo                 be installed. Otherwise builds will be installed to: !-install!
    echo(
    echo   -build BUILD_DIR : The optional location where the third-party libraries of Ocean will
    echo                 be built. Otherwise builds will be installed to: !-build!
    echo(
    echo   -config BUILD_CONFIG : The optional build configs to be built; valid values are:
    echo                 Multiple values must be separated by spaces (inside double quotes^).
    echo                 Default value if nothing is specified: !-config!
    echo(
    echo   -link LINKING_TYPE : The optional linking type for which will be built; valid values are:
    echo                 Multiple values must be separated by spaces (inside double quotes^).
    echo                 Default value if nothing is specified: !-link!
    echo(
    echo   -third-party TP_DIR : The base location where Ocean's third-party dependencies are installed,
    echo                 if they were built manually. If not, CMake will search standard system locations
    echo                 for compatible third-party libraries.
    echo(
    echo   -sdk ANDROID_SDK_VERSION : Default value is %ANDROID_SDK_VERSION%
    echo(
    echo   -quest : If specified, builds will be specialized for Quest apps. Otherwise generic Android is assumed.
    echo(
    echo   -archive ARCHIVE : If specified, this will copy the contents of INSTALL_DIR after the build
    echo                 into a ZIP archive; the path to this archive must exist.
    echo(
    echo   -h : This summary
    echo(
    exit /b
)

set ENABLE_QUEST=
if !-quest! == 1 (
  set ENABLE_QUEST=-DOCEAN_ENABLE_QUEST=TRUE
)

set BUILD_FAILURES=

for %%a in (!-android_abi!) do (
  set ANDROID_ABI=%%a
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
        set bibase=%OCEAN_PLATFORM%\!ANDROID_ABI!_static_!BUILD_TYPE_LOWER!
      ) else if /I %%l==shared (
        set BUILD_SHARED_LIBS=ON
        set bibase=%OCEAN_PLATFORM%\!ANDROID_ABI!_shared_!BUILD_TYPE_LOWER!
      ) else (
        echo Invalid link mode %%l
        exit /b
      )

      call :normalize_path !-build!
      set BUILD_DIRECTORY=!NORMEDPATH!\!bibase!
      call :normalize_path !-install!
      set INSTALL_DIRECTORY=!NORMEDPATH!\!bibase!

      call :normalize_path !-third-party!
      set TPFWD=!NORMEDPATH:\=/!
      set TPDIR=!TPFWD!/!bibase!
      set TPSPEC=-DCMAKE_PREFIX_PATH="!TPDIR!" -DCMAKE_MODULE_PATH="!TPDIR!" -DCMAKE_FIND_ROOT_PATH="!TPDIR!"

      echo BUILD_TYPE           !BUILD_TYPE!
      echo BUILD_SHARED_LIBS    !BUILD_SHARED_LIBS!
      echo BUILD_DIRECTORY      !BUILD_DIRECTORY!
      echo INSTALL_DIRECTORY    !INSTALL_DIRECTORY!

      call :run_build
    )
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
cmake -G"Ninja" ^
      -S "%OCEAN_SOURCE_DIR%" ^
      -B "!BUILD_DIRECTORY!" ^
      "-DCMAKE_INSTALL_PREFIX=!INSTALL_DIRECTORY!" ^
      -DCMAKE_BUILD_TYPE=!BUILD_TYPE! ^
      -DBUILD_SHARED_LIBS=!BUILD_SHARED_LIBS! ^
      -DANDROID_ABI=!ANDROID_ABI! ^
      -DANDROID_PLATFORM=!-sdk! ^
      -DCMAKE_ANDROID_ARCH_ABI=!ANDROID_ABI! ^
      -DCMAKE_ANDROID_STL_TYPE=c++_static ^
      -DCMAKE_ANDROID_NDK=%ANDROID_NDK% ^
      -DCMAKE_SYSTEM_NAME=Android ^
      -DCMAKE_TOOLCHAIN_FILE=%ANDROID_NDK%\build\cmake\android.toolchain.cmake ^
      !TPSPEC! ^
      !ENABLE_QUEST!

cmake --build "!BUILD_DIRECTORY!" --config !BUILD_TYPE! --target install

@echo off
if %errorlevel% neq 0 (
    set BUILD_FAILURES=%BUILD_FAILURES% !bibase!
    exit /b 1
) else (
    exit /b 0
)

:normalize_path
set NORMEDPATH=%~f1
exit /b
