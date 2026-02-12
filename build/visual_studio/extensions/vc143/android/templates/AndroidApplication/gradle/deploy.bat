@echo off
REM Copyright (c) Meta Platforms, Inc. and affiliates.
REM
REM This source code is licensed under the MIT license found in the
REM LICENSE file in the root directory of this source tree.

REM Ocean Android Deployment Script
REM This script installs and launches the Android app via ADB

setlocal enabledelayedexpansion

REM Get the project directory (passed as first argument or use current directory)
set "PROJECT_DIR=%~1"
if "%PROJECT_DIR%"=="" set "PROJECT_DIR=%~dp0"

REM Get the Android package name (passed as second argument)
set "PACKAGE_NAME=%~2"
if "%PACKAGE_NAME%"=="" (
    echo Warning: Package name not provided. App will be installed but not launched.
)

REM Find ADB
set "ADB_PATH=%LOCALAPPDATA%\Android\Sdk\platform-tools\adb.exe"
if not exist "%ADB_PATH%" (
    echo Error: ADB not found at %ADB_PATH%
    echo Please install Android SDK or set ANDROID_HOME environment variable.
    pause
    exit /b 1
)

REM Find the APK
set "APK_PATH=%PROJECT_DIR%app\build\outputs\apk\debug\app-debug.apk"
if not exist "%APK_PATH%" (
    echo Error: APK not found at %APK_PATH%
    echo Please build the project first using Ctrl+Shift+B.
    pause
    exit /b 1
)

echo ========================================
echo Ocean Android Deployment
echo ========================================
echo APK: %APK_PATH%
echo.

REM Check for connected devices
echo Checking for connected devices...
"%ADB_PATH%" devices | findstr /R "device$" >nul
if errorlevel 1 (
    echo Error: No Android device connected.
    echo Please connect a device with USB debugging enabled.
    pause
    exit /b 1
)

REM Install the APK
echo Installing APK...
"%ADB_PATH%" install -r "%APK_PATH%"
if errorlevel 1 (
    echo Error: Failed to install APK.
    pause
    exit /b 1
)

REM Launch the app
echo Launching app...
if "%PACKAGE_NAME%"=="" (
    echo Skipping launch: package name not provided.
) else (
    "%ADB_PATH%" shell am start -n %PACKAGE_NAME%/.MainActivity
    if errorlevel 1 (
        echo Warning: Failed to launch app automatically.
    )
)

echo.
echo ========================================
echo Deployment complete!
echo ========================================
