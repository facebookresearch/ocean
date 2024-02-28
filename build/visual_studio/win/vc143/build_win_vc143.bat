@REM (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

@echo off

echo.
echo Start building WIN vc143...
echo.
echo.
echo.

set PATH=%PATH%;C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin

echo 1 / 12 Build Shared Debug x86...
echo.
msbuild.exe "win_vc143.sln" /t:build /m:8 /v:m /clp:WarningsOnly /clp:ErrorsOnly /clp:Summary /p:configuration="Shared Debug" /p:platform="Win32"
echo.
echo.

echo 2 / 12 Build Shared Profile x86...
echo.
msbuild.exe "win_vc143.sln" /t:build /m:8 /v:m /clp:WarningsOnly /clp:ErrorsOnly /clp:Summary /p:configuration="Shared Profile" /p:platform="Win32"
echo.
echo.

echo 3 / 12 Build Shared Release x86...
echo.
msbuild.exe "win_vc143.sln" /t:build /m:8 /v:m /clp:WarningsOnly /clp:ErrorsOnly /clp:Summary /p:configuration="Shared Release" /p:platform="Win32"
echo.
echo.

echo 4 / 12 Build Static Debug x86...
echo.
msbuild.exe "win_vc143.sln" /t:build /m:8 /v:m /clp:WarningsOnly /clp:ErrorsOnly /clp:Summary /p:configuration="Static Debug" /p:platform="Win32"
echo.
echo.

echo 5 / 12 Build Static Profile x86...
echo.
msbuild.exe "win_vc143.sln" /t:build /m:8 /v:m /clp:WarningsOnly /clp:ErrorsOnly /clp:Summary /p:configuration="Static Profile" /p:platform="Win32"
echo.
echo.

echo 6 / 12 Build Static Release x86...
echo.
msbuild.exe "win_vc143.sln" /t:build /m:8 /v:m /clp:WarningsOnly /clp:ErrorsOnly /clp:Summary /p:configuration="Static Release" /p:platform="Win32"
echo.
echo.

echo 7 / 12 Build Shared Debug x64...
echo.
msbuild.exe "win_vc143.sln" /t:build /m:8 /v:m /clp:WarningsOnly /clp:ErrorsOnly /clp:Summary /p:configuration="Shared Debug" /p:platform="x64"
echo.
echo.

echo 8 / 12 Build Shared Profile x64...
echo.
msbuild.exe "win_vc143.sln" /t:build /m:8 /v:m /clp:WarningsOnly /clp:ErrorsOnly /clp:Summary /p:configuration="Shared Profile" /p:platform="x64"
echo.
echo.

echo 9 / 12 Build Shared Release x64...
echo.
msbuild.exe "win_vc143.sln" /t:build /m:8 /v:m /clp:WarningsOnly /clp:ErrorsOnly /clp:Summary /p:configuration="Shared Release" /p:platform="x64"
echo.
echo.

echo 10 / 12 Build Static Debug x64...
echo.
msbuild.exe "win_vc143.sln" /t:build /m:8 /v:m /clp:WarningsOnly /clp:ErrorsOnly /clp:Summary /p:configuration="Static Debug" /p:platform="x64"
echo.
echo.

echo 11 / 12 Build Static Profile x64...
echo.
msbuild.exe "win_vc143.sln" /t:build /m:8 /v:m /clp:WarningsOnly /clp:ErrorsOnly /clp:Summary /p:configuration="Static Profile" /p:platform="x64"
echo.
echo.

echo 12 / 12 Build Static Release x64...
echo.
msbuild.exe "win_vc143.sln" /t:build /m:8 /v:m /clp:WarningsOnly /clp:ErrorsOnly /clp:Summary /p:configuration="Static Release" /p:platform="x64"
echo.
echo.

if not "%1" == "nopause" (
pause
)
