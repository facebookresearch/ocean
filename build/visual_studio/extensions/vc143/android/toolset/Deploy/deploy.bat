REM Copyright (c) Meta Platforms, Inc. and affiliates.
REM
REM This source code is licensed under the MIT license found in the
REM LICENSE file in the root directory of this source tree.

@echo off

REM Ocean Android Deployment Script
REM This script installs and launches the Android app via ADB

setlocal enabledelayedexpansion

REM Log file for diagnostics
set "LOG_FILE=%LOCALAPPDATA%\OceanAndroidExtension\deploy.log"
if not exist "%LOCALAPPDATA%\OceanAndroidExtension" mkdir "%LOCALAPPDATA%\OceanAndroidExtension"
echo [%date% %time%] Deploy started > "%LOG_FILE%"

echo ========================================
echo Ocean Android Deployment
echo ========================================
echo.

REM Get the project directory (passed as first argument or use current directory)
set "PROJECT_DIR=%~1"
if "%PROJECT_DIR%"=="" set "PROJECT_DIR=%~dp0"
echo Project Dir: %PROJECT_DIR%
echo [%date% %time%] Project Dir: %PROJECT_DIR% >> "%LOG_FILE%"

REM Get the Android package name (passed as second argument)
set "PACKAGE_NAME=%~2"
echo Package Name: %PACKAGE_NAME%
echo [%date% %time%] Package Name: %PACKAGE_NAME% >> "%LOG_FILE%"
if "%PACKAGE_NAME%"=="" (
    echo Warning: Package name not provided. App will be installed but not launched.
)

REM Find ADB - prefer ANDROID_HOME, fall back to default location
if defined ANDROID_HOME (
    set "ADB_PATH=%ANDROID_HOME%\platform-tools\adb.exe"
) else (
    set "ADB_PATH=%LOCALAPPDATA%\Android\Sdk\platform-tools\adb.exe"
)
echo ADB Path: %ADB_PATH%
echo [%date% %time%] ADB Path: %ADB_PATH% >> "%LOG_FILE%"
if not exist "%ADB_PATH%" (
    echo.
    echo Error: ADB not found at %ADB_PATH%
    echo Please install Android SDK or set the ANDROID_HOME environment variable.
    echo [%date% %time%] ERROR: ADB not found at %ADB_PATH% >> "%LOG_FILE%"
    timeout /t 30 /nobreak
    exit /b 1
)

REM Find the APK (third argument, or default location)
set "APK_PATH=%~3"
if "%APK_PATH%"=="" set "APK_PATH=%PROJECT_DIR%app\build\outputs\apk\debug\app-debug.apk"
echo APK Path: %APK_PATH%
echo [%date% %time%] APK Path: %APK_PATH% >> "%LOG_FILE%"
if not exist "%APK_PATH%" (
    echo.
    echo Error: APK not found at %APK_PATH%
    echo Please build the project first using Ctrl+Shift+B.
    echo [%date% %time%] ERROR: APK not found at %APK_PATH% >> "%LOG_FILE%"
    timeout /t 30 /nobreak
    exit /b 1
)

echo.

REM Check for connected devices
echo Checking for connected devices...
"%ADB_PATH%" devices > "%TEMP%\adb_devices.txt" 2>&1
type "%TEMP%\adb_devices.txt"
echo [%date% %time%] ADB devices output: >> "%LOG_FILE%"
type "%TEMP%\adb_devices.txt" >> "%LOG_FILE%"
findstr /R "device$" "%TEMP%\adb_devices.txt" >nul
if errorlevel 1 (
    echo.
    echo Error: No Android device connected.
    echo Please connect a device with USB debugging enabled.
    echo [%date% %time%] ERROR: No Android device connected >> "%LOG_FILE%"
    timeout /t 30 /nobreak
    exit /b 1
)

REM Install the APK
echo Installing APK...
echo [%date% %time%] Installing APK... >> "%LOG_FILE%"
"%ADB_PATH%" install -r "%APK_PATH%"
if errorlevel 1 (
    echo.
    echo Error: Failed to install APK.
    echo [%date% %time%] ERROR: Failed to install APK >> "%LOG_FILE%"
    timeout /t 30 /nobreak
    exit /b 1
)

REM Launch the app
echo Launching app...
if "%PACKAGE_NAME%"=="" (
    echo Skipping launch: package name not provided.
    echo [%date% %time%] Skipping launch: no package name >> "%LOG_FILE%"
) else (
    echo [%date% %time%] Launching %PACKAGE_NAME%/.MainActivity >> "%LOG_FILE%"
    "%ADB_PATH%" shell am start -n %PACKAGE_NAME%/.MainActivity
    if errorlevel 1 (
        echo Warning: Failed to launch app automatically.
        echo [%date% %time%] WARNING: Failed to launch app >> "%LOG_FILE%"
    )
)

echo.
echo ========================================
echo Deployment complete!
echo ========================================
echo [%date% %time%] Deployment complete >> "%LOG_FILE%"
echo.
echo Log file: %LOG_FILE%
timeout /t 5 /nobreak
