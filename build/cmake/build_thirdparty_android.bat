@REM Copyright (c) Meta Platforms, Inc. and affiliates.
@REM
@REM This source code is licensed under the MIT license found in the
@REM LICENSE file in the root directory of this source tree.

echo off

setlocal

@REM Determine the location of the source directory from the location of this script
set OCEAN_THIRD_PARTY_SOURCE_DIR=%~dp0..\..\build\cmake\third-party

set BUILD_FAILURES=

set VCVARSALL="C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat"

if "%ANDROID_NDK%" == "" (
    echo "ERROR: Set ANDROID_NDK to the location of your Android NDK installation."
    exit /b 1
)

if "%JAVA_HOME%" == "" (
    echo "ERROR: Set JAVA_HOME to the location of your Java installation."
    exit /b 1
)

set ANDROID_SDK_VERSION=android-34

@REM SET PATH=%PATH%;C:\Users\aschneider1\Desktop
@REM SET PATH=%PATH%;C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC\14.39.33519\bin\Hostx64\x64

@REM
@REM arm64-v8a
@REM

set ANDROID_ABI=arm64-v8a
@REM call %VCVARSALL% x64_arm64

@REM Static, debug
set BUILD_TYPE=Debug
set BUILD_SHARED_LIBS=OFF
set BUILD_DIRECTORY=C:\tmp\ocean\build\android\third-party\%ANDROID_ABI%_static_%BUILD_TYPE%
set INSTALL_DIRECTORY=C:\tmp\ocean\install\android\%ANDROID_ABI%_static_%BUILD_TYPE%
call :run_build

@REM Static, release
set BUILD_TYPE=Release
set BUILD_SHARED_LIBS=OFF
set BUILD_DIRECTORY=C:\tmp\ocean\build\android\third-party\%ANDROID_ABI%_static_%BUILD_TYPE%
set INSTALL_DIRECTORY=C:\tmp\ocean\install\android\%ANDROID_ABI%_static_%BUILD_TYPE%
call :run_build

@REM
@REM armeabi-v7a
@REM

set ANDROID_ABI=armeabi-v7a
@REM call %VCVARSALL% x64_arm

@REM Static, debug
set BUILD_TYPE=Debug
set BUILD_SHARED_LIBS=OFF
set BUILD_DIRECTORY=C:\tmp\ocean\build\android\third-party\%ANDROID_ABI%_static_%BUILD_TYPE%
set INSTALL_DIRECTORY=C:\tmp\ocean\install\android\%ANDROID_ABI%_static_%BUILD_TYPE%
call :run_build

@REM Static, release
set BUILD_TYPE=Release
set BUILD_SHARED_LIBS=OFF
set BUILD_DIRECTORY=C:\tmp\ocean\build\android\third-party\%ANDROID_ABI%_static_%BUILD_TYPE%
set INSTALL_DIRECTORY=C:\tmp\ocean\install\android\%ANDROID_ABI%_static_%BUILD_TYPE%
call :run_build

@REM
@REM x86_64
@REM

set ANDROID_ABI=x86_64
@REM call %VCVARSALL% x64

@REM Static, debug
set BUILD_TYPE=Debug
set BUILD_SHARED_LIBS=OFF
set BUILD_DIRECTORY=C:\tmp\ocean\build\android\third-party\%ANDROID_ABI%_static_%BUILD_TYPE%
set INSTALL_DIRECTORY=C:\tmp\ocean\install\android\%ANDROID_ABI%_static_%BUILD_TYPE%
call :run_build

@REM Static, release
set BUILD_TYPE=Release
set BUILD_SHARED_LIBS=OFF
set BUILD_DIRECTORY=C:\tmp\ocean\build\android\third-party\%ANDROID_ABI%_static_%BUILD_TYPE%
set INSTALL_DIRECTORY=C:\tmp\ocean\install\android\%ANDROID_ABI%_static_%BUILD_TYPE%
call :run_build

@REM
@REM x86
@REM

set ANDROID_ABI=x86
@REM call %VCVARSALL% x64_x86

@REM Static, debug
set BUILD_TYPE=Debug
set BUILD_SHARED_LIBS=OFF
set BUILD_DIRECTORY=C:\tmp\ocean\build\android\third-party\%ANDROID_ABI%_static_%BUILD_TYPE%
set INSTALL_DIRECTORY=C:\tmp\ocean\install\android\%ANDROID_ABI%_static_%BUILD_TYPE%
call :run_build

@REM Static, release
set BUILD_TYPE=Release
set BUILD_SHARED_LIBS=OFF
set BUILD_DIRECTORY=C:\tmp\ocean\build\android\third-party\%ANDROID_ABI%_static_%BUILD_TYPE%
set INSTALL_DIRECTORY=C:\tmp\ocean\install\android\%ANDROID_ABI%_static_%BUILD_TYPE%
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
call %OCEAN_THIRD_PARTY_SOURCE_DIR%\build_deps.bat android %OCEAN_THIRD_PARTY_SOURCE_DIR% %BUILD_DIRECTORY% "-" ^
        "-GNMake Makefiles" ^
        "-DCMAKE_INSTALL_PREFIX=%INSTALL_DIRECTORY%" ^
        "-DCMAKE_BUILD_TYPE=%BUILD_TYPE%" ^
        "-DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS%" ^
        "-DANDROID_ABI=%ANDROID_ABI%" ^
        "-DANDROID_PLATFORM=%ANDROID_SDK_VERSION%" ^
        "-DCMAKE_ANDROID_ARCH_ABI=%ANDROID_ABI%" ^
        "-DCMAKE_ANDROID_STL_TYPE=c++_static" ^
        "-DCMAKE_ANDROID_NDK=%ANDROID_NDK%" ^
        "-DCMAKE_SYSTEM_NAME=Android" ^
        "-DCMAKE_TOOLCHAIN_FILE=%ANDROID_NDK%\build\cmake\android.toolchain.cmake"

@echo off
if %errorlevel% neq 0 (
    set BUILD_FAILURES=%BUILD_FAILURES% %LIBRARY_TYPE%_%BUILD_TYPE%_pass%BUILD_PASS%
    exit /b 1
) else (
    exit /b 0
)
