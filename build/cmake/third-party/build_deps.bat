@REM Copyright (c) Meta Platforms, Inc. and affiliates.
@REM
@REM This source code is licensed under the MIT license found in the
@REM LICENSE file in the root directory of this source tree.

@echo off

setlocal

set PLATFORM=%1
set OCEAN_THIRD_PARTY_SOURCE_DIR=%~2
set BUILD_DIRECTORY_BASE=%~3
set EXTRA_BUILD_FLAGS=%4

shift
shift
shift
shift

set a1=%1
set a2=%2
set a3=%3
set a4=%4
set a5=%5
set a6=%6
set a7=%7
set a8=%8
set a9=%9

shift
shift
shift
shift
shift
shift
shift
shift
shift

for /F "eol=# usebackq delims=" %%d in ("%OCEAN_THIRD_PARTY_SOURCE_DIR%\dependencies_allplatforms.txt" "%OCEAN_THIRD_PARTY_SOURCE_DIR%\dependencies_%PLATFORM%.txt") do (
    echo %%d
    cmake -S "%OCEAN_THIRD_PARTY_SOURCE_DIR%" -B "%BUILD_DIRECTORY_BASE%\%%d" -DINCLUDED_DEP_NAME=%%d %a1% %a2% %a3% %a4% %a5% %a6% %a7% %a8% %a9% %1 %2 %3 %4 %5 %6 %7 %8 %9
    if ERRORLEVEL 1 (
        exit /b 1
    )
    cmake --build "%BUILD_DIRECTORY_BASE%\%%d" --config %BUILD_TYPE% --target install -- %EXTRA_BUILD_FLAGS%
    if ERRORLEVEL 1 (
        exit /b 2
    )
)
