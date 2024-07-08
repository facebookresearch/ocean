@REM Copyright (c) Meta Platforms, Inc. and affiliates.
@REM
@REM This source code is licensed under the MIT license found in the
@REM LICENSE file in the root directory of this source tree.

@echo off

echo.
echo Start building android_vc143...
echo.
echo.
echo.

set PATH=%PATH%;C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin

echo 1 / 4 Build Static Debug ARM...
echo.
msbuild.exe "android_vc143.sln" /t:build /m:8 /v:m /clp:WarningsOnly /clp:ErrorsOnly /clp:Summary /p:configuration="Static Debug" /p:platform="ARM"
echo.
echo.

echo 2 / 4 Build Static Debug ARM64...
echo.
msbuild.exe "android_vc143.sln" /t:build /m:8 /v:m /clp:WarningsOnly /clp:ErrorsOnly /clp:Summary /p:configuration="Static Debug" /p:platform="ARM64"
echo.
echo.

echo 3 / 4 Build Static Release ARM...
echo.
msbuild.exe "android_vc143.sln" /t:build /m:8 /v:m /clp:WarningsOnly /clp:ErrorsOnly /clp:Summary /p:configuration="Static Release" /p:platform="ARM"
echo.
echo.

echo 4 / 4 Build Static Release ARM64...
echo.
msbuild.exe "android_vc143.sln" /t:build /m:8 /v:m /clp:WarningsOnly /clp:ErrorsOnly /clp:Summary /p:configuration="Static Release" /p:platform="ARM64"
echo.
echo.

if not "%1" == "nopause" (
pause
)
