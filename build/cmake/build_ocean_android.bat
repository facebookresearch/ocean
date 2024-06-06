@REM Copyright (c) Meta Platforms, Inc. and affiliates.
@REM
@REM This source code is licensed under the MIT license found in the
@REM LICENSE file in the root directory of this source tree.

@echo off

@REM Determine the location of the source directory from the location of this script
set OCEAN_SOURCE_DIR=%~dp0..\..
set OCEAN_BUILD_ROOT_DIRECTORY=C:\tmp\ocean\build\and
set OCEAN_INSTALL_ROOT_DIRECTORY=C:\tmp\ocean\install\and

setlocal

if "%ANDROID_NDK%" == "" (
    echo "ERROR: Set ANDROID_NDK to the location of your Android NDK installation."
    exit /b 1
)

if "%JAVA_HOME%" == "" (
    echo "ERROR: Set JAVA_HOME to the location of your Java installation."
    exit /b 1
)

set THIRD_PARTY_ROOT_DIRECTORY=C:\tmp\ocean\install\and\

set ANDROID_SDK_VERSION=android-34

set BUILD_FAILURES=

set LIBRARY_TYPE=static
set BUILD_SHARED_LIBS=OFF

@REM
@REM arm64-v8a
@REM
set ANDROID_ABI=arm64-v8a

@REM Static, debug
set BUILD_TYPE=Debug
call :run_build

@REM Static, release
set BUILD_TYPE=Release
call :run_build

@REM
@REM armeabi-v7a
@REM
set ANDROID_ABI=armeabi-v7a

@REM Static, debug
set BUILD_TYPE=Debug
call :run_build

@REM Static, release
set BUILD_TYPE=Release
call :run_build

@REM
@REM x86_64
@REM
set ANDROID_ABI=x86_64

@REM Static, debug
set BUILD_TYPE=Debug
call :run_build

@REM Static, release
set BUILD_TYPE=Release
call :run_build

@REM
@REM x86
@REM
set ANDROID_ABI=x86

@REM Static, debug
set BUILD_TYPE=Debug
call :run_build

@REM Static, release
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

set OCEAN_BUILD_DIRECTORY=%OCEAN_BUILD_ROOT_DIRECTORY%\%ANDROID_ABI%_%LIBRARY_TYPE%_%BUILD_TYPE%
set OCEAN_INSTALL_DIRECTORY=%OCEAN_INSTALL_ROOT_DIRECTORY%\%ANDROID_ABI%_%LIBRARY_TYPE%_%BUILD_TYPE%
set OCEAN_THIRD_PARTY_DIRECTORY=%THIRD_PARTY_ROOT_DIRECTORY%\%ANDROID_ABI%_%LIBRARY_TYPE%_%BUILD_TYPE%

echo;
echo BUILD_TYPE: %BUILD_TYPE%
echo LIBRARY_TYPE: %LIBRARY_TYPE%
echo;
echo OCEAN_BUILD_DIRECTORY: %OCEAN_BUILD_DIRECTORY%
echo OCEAN_INSTALL_DIRECTORY: %OCEAN_INSTALL_DIRECTORY%
echo OCEAN_THIRD_PARTY_DIRECTORY: %OCEAN_THIRD_PARTY_DIRECTORY%

cmake -G"Ninja" ^
      -S %OCEAN_SOURCE_DIR% ^
      -B %OCEAN_BUILD_DIRECTORY% ^
      -DCMAKE_INSTALL_PREFIX=%OCEAN_INSTALL_DIRECTORY% ^
      -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
      -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS% ^
      -DOCEAN_THIRD_PARTY_ROOT_FROM_GRADLE=%OCEAN_THIRD_PARTY_DIRECTORY% ^
      -DANDROID_ABI=%ANDROID_ABI% ^
      -DANDROID_PLATFORM=%ANDROID_SDK_VERSION% ^
      -DCMAKE_ANDROID_ARCH_ABI=%ANDROID_ABI% ^
      -DCMAKE_ANDROID_STL_TYPE=c++_static ^
      -DCMAKE_ANDROID_NDK=%ANDROID_NDK% ^
      -DCMAKE_SYSTEM_NAME=Android ^
      -DCMAKE_TOOLCHAIN_FILE=%ANDROID_NDK%\build\cmake\android.toolchain.cmake

cmake --build %OCEAN_BUILD_DIRECTORY% --config %BUILD_TYPE% --target install

echo;
echo;

if %errorlevel% neq 0 (
    set BUILD_FAILURES=%BUILD_FAILURES% %LIBRARY_TYPE%_%BUILD_TYPE%
    exit /b 1
) else (
    exit /b 0
)
