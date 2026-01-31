# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

<#
.SYNOPSIS
    Build script for third-party libraries required by Ocean on Windows.

.DESCRIPTION
    This script builds all third-party dependencies required by Ocean for Windows.
    It supports building multiple configurations (Debug/Release) and linking types
    (static/shared).

.PARAMETER Install
    Installation directory. Default: .\ocean_install_thirdparty

.PARAMETER Build
    Build directory. Default: .\ocean_build_thirdparty

.PARAMETER Config
    Build configuration(s): debug, release, or both (comma-separated).
    Default: debug,release

.PARAMETER Link
    Linking type(s): static, shared, or both (comma-separated).
    Default: static

.PARAMETER Subdivide
    Install each library into its own subdirectory. When enabled, libraries will
    be installed to {InstallDir}\library_name\{lib,include,...}. Default: OFF

.PARAMETER Archive
    If specified, create a ZIP archive of the install directory after the build.

.PARAMETER LogLevel
    CMake log level: ERROR, WARNING, NOTICE, STATUS, VERBOSE, DEBUG, TRACE.
    Default: ERROR

.PARAMETER Generator
    CMake generator to use. For Windows, you can specify a Visual Studio version:
    "Visual Studio 17 2022", "Visual Studio 16 2019", etc.
    By default, CMake auto-detects the newest available Visual Studio.

.EXAMPLE
    .\build_thirdparty_windows.ps1
    Build debug and release static libraries

.EXAMPLE
    .\build_thirdparty_windows.ps1 -Config release -Link static
    Build only release static libraries

.EXAMPLE
    .\build_thirdparty_windows.ps1 -Config "debug,release" -Link "static,shared"
    Build all four combinations

.EXAMPLE
    .\build_thirdparty_windows.ps1 -Generator "Visual Studio 16 2019"
    Build using Visual Studio 2019

.EXAMPLE
    .\build_thirdparty_windows.ps1 -Subdivide ON -LogLevel STATUS
    Build with per-library subdirectories and verbose output

.EXAMPLE
    .\build_thirdparty_windows.ps1 -Archive "thirdparty.zip"
    Build and create archive
#>

[CmdletBinding()]
param(
    [string]$Install = ".\ocean_install_thirdparty",
    [string]$Build = ".\ocean_build_thirdparty",
    [string]$Config = "debug,release",
    [string]$Link = "static",

    [ValidateSet("ON", "OFF")]
    [string]$Subdivide = "OFF",

    [string]$Archive = "",

    [ValidateSet("ERROR", "WARNING", "NOTICE", "STATUS", "VERBOSE", "DEBUG", "TRACE")]
    [string]$LogLevel = "ERROR",

    [string]$Generator = ""
)

$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ThirdPartySourceDir = Join-Path $ScriptDir "third-party"
$Platform = "windows"

# Dot-source common functions
. (Join-Path $ScriptDir "build_common.ps1")

# Check prerequisites
if (-not (Test-CMake)) {
    exit 1
}

# Detect architecture
$Arch = Get-DefaultArchitecture

# Detect VS toolset
$vsToolset = Get-VSToolsetVersion -Generator $Generator
if ($vsToolset) {
    Write-Host "Detected Visual Studio toolset: $vsToolset" -ForegroundColor Green
} else {
    Write-Host "Warning: Could not detect Visual Studio toolset version, using default naming." -ForegroundColor Yellow
    $vsToolset = ""
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
Write-Host "Ocean Third-Party Build Configuration (Windows)" -ForegroundColor Cyan
Write-Host "================================================" -ForegroundColor Cyan
Write-Host "  Platform:        $Platform"
Write-Host "  Architecture:    $Arch"
if ($vsToolset) {
    Write-Host "  VS Toolset:      $vsToolset" -ForegroundColor Magenta
}
Write-Host "  Configurations:  $Config"
Write-Host "  Link types:      $Link"
Write-Host "  Build dir:       $Build"
Write-Host "  Install dir:     $Install"
Write-Host "  Subdivide:       $Subdivide"
Write-Host "  Log level:       $LogLevel"
if ($Generator) {
    Write-Host "  Generator:       $Generator" -ForegroundColor Magenta
} else {
    Write-Host "  Generator:       (auto-detect)"
}
Write-Host ""
Write-Host "Builds to run:" -ForegroundColor Yellow
foreach ($c in $configs) {
    foreach ($l in $linkTypes) {
        $vsToolsetSuffix = if ($vsToolset) { "_$vsToolset" } else { "" }
        Write-Host "  - $Platform\${Arch}${vsToolsetSuffix}_${l}_${c}"
    }
}
Write-Host ""

# Track failed builds
$failedBuilds = @()

# Build each configuration
foreach ($c in $configs) {
    foreach ($l in $linkTypes) {
        # Determine build type
        $buildType = if ($c -eq "debug") { "Debug" } else { "Release" }
        $buildSharedLibs = if ($l -eq "shared") { "ON" } else { "OFF" }

        # Construct directory names
        $vsToolsetSuffix = if ($vsToolset) { "_$vsToolset" } else { "" }
        $configDirName = "${Arch}${vsToolsetSuffix}_${l}_${c}"
        $buildDir = Join-Path $Build "$Platform\$configDirName"
        $installDir = Join-Path $Install "$Platform\$configDirName"

        Write-Host ""
        Write-Host "========================================" -ForegroundColor Cyan
        Write-Host "Building: $Platform\$configDirName" -ForegroundColor Cyan
        Write-Host "========================================" -ForegroundColor Cyan
        Write-Host ""
        Write-Host "BUILD_TYPE           $buildType"
        Write-Host "BUILD_SHARED_LIBS    $buildSharedLibs"
        Write-Host "BUILD_DIRECTORY      $buildDir"
        Write-Host "INSTALL_DIRECTORY    $installDir"

        # Build extra CMake args
        $extraCMakeArgs = @(
            "-DCMAKE_CONFIGURATION_TYPES=$buildType",
            "-DCMAKE_POLICY_VERSION_MINIMUM=3.5"
        )

        # Build extra build flags for MSBuild
        $extraBuildFlags = "/m:16"
        if ($LogLevel -eq "ERROR") {
            $extraBuildFlags = "/m:16 /v:q"
        }

        # Invoke build_deps.ps1
        $buildDepsScript = Join-Path $ThirdPartySourceDir "build_deps.ps1"

        $buildParams = @{
            Platform = $Platform
            SourceDir = $ThirdPartySourceDir
            BuildDir = $buildDir
            InstallPrefix = $installDir
            BuildType = $buildType
            BuildSharedLibs = $buildSharedLibs
            Subdivide = $Subdivide
            LogLevel = $LogLevel
            ExtraBuildFlags = $extraBuildFlags
            ExtraCMakeArgs = $extraCMakeArgs
        }

        if ($Generator) {
            $buildParams["Generator"] = $Generator
        }

        & $buildDepsScript @buildParams

        if ($LASTEXITCODE -ne 0) {
            $failedBuilds += "$Platform\$configDirName"
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

    Write-Host ""
    Write-Host "Install locations:" -ForegroundColor Yellow
    foreach ($c in $configs) {
        foreach ($l in $linkTypes) {
            $vsToolsetSuffix = if ($vsToolset) { "_$vsToolset" } else { "" }
            Write-Host "  - $Install\$Platform\${Arch}${vsToolsetSuffix}_${l}_${c}"
        }
    }
    exit 0
} else {
    Write-Host "Some builds failed:" -ForegroundColor Red
    foreach ($build in $failedBuilds) {
        Write-Host "  - $build" -ForegroundColor Red
    }
    exit 1
}
