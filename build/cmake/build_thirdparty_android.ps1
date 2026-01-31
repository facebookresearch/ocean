# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

<#
.SYNOPSIS
    Build script for third-party libraries required by Ocean for Android (cross-compile on Windows).

.DESCRIPTION
    This script cross-compiles all third-party dependencies required by Ocean for Android.
    It supports building multiple ABIs, configurations (Debug/Release), and linking types
    (static/shared).

    Prerequisites:
    - ANDROID_HOME environment variable must be set to the Android SDK location
    - ANDROID_NDK environment variable must be set to the Android NDK location
    - JAVA_HOME environment variable must be set to the Java installation location
    - Ninja build system must be installed and in PATH

.PARAMETER Install
    Installation directory. Default: .\bin\cmake\3rdparty

.PARAMETER Build
    Build directory. Default: .\bin\cmake\3rdparty\tmp

.PARAMETER ABI
    Android ABI(s) to build: arm64-v8a, armeabi-v7a, x86_64, x86 (comma-separated).
    Default: arm64-v8a

.PARAMETER Config
    Build configuration(s): debug, release, or both (comma-separated).
    Default: debug,release

.PARAMETER Link
    Linking type(s): static, shared, or both (comma-separated).
    Default: static

.PARAMETER Sdk
    Android SDK/API version. Default: android-34

.PARAMETER Subdivide
    Install each library into its own subdirectory. When enabled, libraries will
    be installed to {InstallDir}\library_name\{lib,include,...}. Default: OFF

.PARAMETER ForVisualStudio
    If specified, automatically enables subdivision and reorganizes the output
    into a platform-first folder structure suitable for Visual Studio projects.
    Output will be placed in xplat\ocean\3rdparty\android\h\ and
    xplat\ocean\3rdparty\android\lib\{config}\.

.PARAMETER Archive
    If specified, create a ZIP archive of the install directory after the build.

.PARAMETER LogLevel
    CMake log level: ERROR, WARNING, NOTICE, STATUS, VERBOSE, DEBUG, TRACE.
    Default: ERROR

.EXAMPLE
    .\build_thirdparty_android.ps1
    Build debug and release static libraries for arm64-v8a

.EXAMPLE
    .\build_thirdparty_android.ps1 -ABI "arm64-v8a,armeabi-v7a,x86_64"
    Build for multiple ABIs

.EXAMPLE
    .\build_thirdparty_android.ps1 -ABI arm64-v8a -Config release -Link static
    Build only release static libraries for arm64

.EXAMPLE
    .\build_thirdparty_android.ps1 -Sdk android-32
    Build targeting Android API level 32

.EXAMPLE
    .\build_thirdparty_android.ps1 -Archive "thirdparty_android.zip"
    Build and create archive
#>

[CmdletBinding()]
param(
    [string]$Install = ".\bin\cmake\3rdparty",
    [string]$Build = ".\bin\cmake\3rdparty\tmp",
    [string]$ABI = "arm64-v8a",
    [string]$Config = "debug,release",
    [string]$Link = "static",
    [string]$Sdk = "android-34",

    [ValidateSet("ON", "OFF")]
    [string]$Subdivide = "OFF",

    [switch]$ForVisualStudio,

    [string]$Archive = "",

    [ValidateSet("ERROR", "WARNING", "NOTICE", "STATUS", "VERBOSE", "DEBUG", "TRACE")]
    [string]$LogLevel = "ERROR"
)

$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ThirdPartySourceDir = Join-Path $ScriptDir "third-party"
$Platform = "android"

# Dot-source common functions
. (Join-Path $ScriptDir "build_common.ps1")

# Handle -ForVisualStudio flag
if ($ForVisualStudio) {
    Write-Host ""
    Write-Host "ForVisualStudio mode enabled - subdivision will be activated" -ForegroundColor Magenta
    $Subdivide = "ON"
}

# Check prerequisites
if (-not (Test-CMake)) {
    exit 1
}

if (-not (Test-Ninja)) {
    exit 1
}

if (-not (Test-AndroidPrerequisites)) {
    exit 1
}

# Check for Long Path support
Test-LongPathSupport | Out-Null

# Parse ABIs
$validABIs = @("arm64-v8a", "armeabi-v7a", "x86_64", "x86")
$abis = $ABI -split "," | ForEach-Object { $_.Trim() }
foreach ($a in $abis) {
    if ($a -notin $validABIs) {
        Write-Host "ERROR: Invalid Android ABI: $a. Valid: $($validABIs -join ', ')" -ForegroundColor Red
        exit 1
    }
}

# Parse configurations
$configs = $Config -split "," | ForEach-Object { $_.Trim().ToLower() }
foreach ($c in $configs) {
    if ($c -notin @("debug", "release")) {
        Write-Host "ERROR: Invalid build configuration: $c. Valid: debug, release" -ForegroundColor Red
        exit 1
    }
}

# Parse link types
$linkTypes = $Link -split "," | ForEach-Object { $_.Trim().ToLower() }
foreach ($l in $linkTypes) {
    if ($l -notin @("static", "shared")) {
        Write-Host "ERROR: Invalid linking type: $l. Valid: static, shared" -ForegroundColor Red
        exit 1
    }
}

# Resolve directories to absolute paths
if (-not (Test-Path $Build)) {
    New-Item -ItemType Directory -Path $Build -Force | Out-Null
}
$Build = (Resolve-Path $Build).Path

if (-not (Test-Path $Install)) {
    New-Item -ItemType Directory -Path $Install -Force | Out-Null
}
$Install = (Resolve-Path $Install).Path

# Display build plan
Write-Host ""
Write-Host "Ocean Third-Party Build Configuration (Android)" -ForegroundColor Cyan
Write-Host "================================================" -ForegroundColor Cyan
Write-Host "  Platform:        $Platform"
Write-Host "  ABIs:            $ABI"
Write-Host "  Android SDK:     $Sdk"
Write-Host "  Configurations:  $Config"
Write-Host "  Link types:      $Link"
Write-Host "  Build dir:       $Build"
Write-Host "  Install dir:     $Install"
Write-Host "  Subdivide:       $Subdivide"
Write-Host "  Log level:       $LogLevel"
Write-Host "  ANDROID_NDK:     $env:ANDROID_NDK"
Write-Host ""
Write-Host "Builds to run:" -ForegroundColor Yellow
foreach ($a in $abis) {
    $arch = Get-AndroidArchFromABI -ABI $a
    foreach ($c in $configs) {
        foreach ($l in $linkTypes) {
            Write-Host "  - $Platform\${arch}_${l}_${c}"
        }
    }
}
Write-Host ""

# Track failed builds
$failedBuilds = @()

# Build each configuration
foreach ($a in $abis) {
    foreach ($c in $configs) {
        foreach ($l in $linkTypes) {
            # Determine build type
            $buildType = if ($c -eq "debug") { "Debug" } else { "Release" }
            $buildSharedLibs = if ($l -eq "shared") { "ON" } else { "OFF" }

            # Map ABI to architecture name (arm64-v8a -> arm64, etc.)
            $arch = Get-AndroidArchFromABI -ABI $a

            # Construct directory names
            $configDirName = "${arch}_${l}_${c}"
            $buildDir = Join-Path $Build "android\$configDirName"
            $installDir = Join-Path $Install "android\$configDirName"

            Write-Host ""
            Write-Host "========================================" -ForegroundColor Cyan
            Write-Host "Building: android\$configDirName (ABI: $a)" -ForegroundColor Cyan
            Write-Host "========================================" -ForegroundColor Cyan
            Write-Host ""
            Write-Host "BUILD_TYPE           $buildType"
            Write-Host "BUILD_SHARED_LIBS    $buildSharedLibs"
            Write-Host "ANDROID_ABI          $a"
            Write-Host "ANDROID_PLATFORM     $Sdk"
            Write-Host "BUILD_DIRECTORY      $buildDir"
            Write-Host "INSTALL_DIRECTORY    $installDir"

            # Build extra CMake args for Android cross-compilation
            $toolchainFile = Join-Path $env:ANDROID_NDK "build\cmake\android.toolchain.cmake"
            $findNinjaAndroidSDK = Join-Path $ScriptDir "FindNinjaAndroidSDK.cmake"

            # Get Ninja path explicitly (Android toolchain may not inherit PATH correctly)
            $ninjaPath = (Get-Command ninja -ErrorAction SilentlyContinue).Source

            $extraCMakeArgs = @(
                "-DANDROID_ABI=$a",
                "-DANDROID_PLATFORM=$Sdk",
                "-DCMAKE_ANDROID_ARCH_ABI=$a",
                "-DCMAKE_ANDROID_STL_TYPE=c++_static",
                "-DCMAKE_ANDROID_NDK=$env:ANDROID_NDK",
                "-DCMAKE_SYSTEM_NAME=Android",
                "-DCMAKE_TOOLCHAIN_FILE=$toolchainFile",
                "-DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=$findNinjaAndroidSDK",
                "-DCMAKE_MAKE_PROGRAM=$ninjaPath",
                "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
            )

            # Invoke build_deps.ps1
            $buildDepsScript = Join-Path $ThirdPartySourceDir "build_deps.ps1"

            $buildParams = @{
                Platform = "android"
                SourceDir = $ThirdPartySourceDir
                BuildDir = $buildDir
                InstallPrefix = $installDir
                BuildType = $buildType
                BuildSharedLibs = $buildSharedLibs
                Subdivide = $Subdivide
                LogLevel = $LogLevel
                Generator = "Ninja"
                ExtraBuildFlags = "-j16"
                ExtraCMakeArgs = $extraCMakeArgs
            }

            & $buildDepsScript @buildParams

            if ($LASTEXITCODE -ne 0) {
                $failedBuilds += "android\$configDirName"
            }
        }
    }
}

# Report results
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Build Summary" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

if ($failedBuilds.Count -eq 0) {
    Write-Host "All builds completed successfully." -ForegroundColor Green

    # Create archive if requested
    if ($Archive) {
        Write-Host ""
        Write-Host "Creating archive: $Archive" -ForegroundColor Yellow
        Push-Location $Install
        try {
            tar -a -c -f $Archive .
            Write-Host "Archive created successfully." -ForegroundColor Green
        } finally {
            Pop-Location
        }
    }

    # Run reorganization script if -ForVisualStudio was specified
    if ($ForVisualStudio) {
        Write-Host ""
        Write-Host "========================================" -ForegroundColor Magenta
        Write-Host "Reorganizing for Visual Studio" -ForegroundColor Magenta
        Write-Host "========================================" -ForegroundColor Magenta

        $reorganizeScript = Join-Path $ScriptDir "reorganize_thirdparty.ps1"
        if (Test-Path $reorganizeScript) {
            & $reorganizeScript -Source $Install -Config $Config -Link $Link -Platform android -ABI $ABI

            if ($LASTEXITCODE -ne 0) {
                Write-Host "ERROR: Reorganization failed." -ForegroundColor Red
                exit 1
            }
        } else {
            Write-Host "ERROR: Reorganization script not found: $reorganizeScript" -ForegroundColor Red
            exit 1
        }
    }

    Write-Host ""
    Write-Host "Install locations:" -ForegroundColor Yellow
    foreach ($a in $abis) {
        $arch = Get-AndroidArchFromABI -ABI $a
        foreach ($c in $configs) {
            foreach ($l in $linkTypes) {
                Write-Host "  - $Install\android\${arch}_${l}_${c}"
            }
        }
    }

    if ($ForVisualStudio) {
        $OceanRoot = Split-Path -Parent (Split-Path -Parent $ScriptDir)
        $vsTargetDir = Join-Path $OceanRoot "3rdparty"
        Write-Host ""
        Write-Host "Visual Studio project directory:" -ForegroundColor Magenta
        Write-Host "  - $vsTargetDir"
    }

    exit 0
} else {
    Write-Host "Some builds failed:" -ForegroundColor Red
    foreach ($build in $failedBuilds) {
        Write-Host "  - $build" -ForegroundColor Red
    }
    exit 1
}
