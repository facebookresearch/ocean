@REM Copyright (c) Meta Platforms, Inc. and affiliates.
@REM
@REM This source code is licensed under the MIT license found in the
@REM LICENSE file in the root directory of this source tree.

@echo off

setlocal

set OCEAN_THIRD_PARTY_SOURCE_DIR=%1
set BUILD_DIRECTORY_BASE=%2
set INSTALL_DIRECTORY=%3
set BUILD_TYPE=%4
set BUILD_SHARED_LIBS=%5

for /F %%d in (%OCEAN_THIRD_PARTY_SOURCE_DIR%\dependencies_allplatforms.txt %OCEAN_THIRD_PARTY_SOURCE_DIR%\dependencies_windows.txt) do (
    echo %%d
    cmake -S %OCEAN_THIRD_PARTY_SOURCE_DIR% -B %BUILD_DIRECTORY_BASE%\%%d -DCMAKE_INSTALL_PREFIX=%INSTALL_DIRECTORY% -DCMAKE_CONFIGURATION_TYPES=%BUILD_TYPE% -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS% -DINCLUDED_DEP_NAME=%%d
    cmake --build %BUILD_DIRECTORY_BASE%\%%d --config %BUILD_TYPE% --target install -- /m:16
)
