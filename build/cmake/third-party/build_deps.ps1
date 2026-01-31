# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

<#
.SYNOPSIS
    Core script to build third-party dependencies for Ocean.

.DESCRIPTION
    This script iterates through the dependency lists and builds each library.
    It is called by the build_thirdparty_*.ps1 scripts and should not be run directly.

.PARAMETER Platform
    Target platform: windows, android

.PARAMETER SourceDir
    Path to the third-party source directory

.PARAMETER BuildDir
    Base directory for build files

.PARAMETER InstallPrefix
    Base directory for installed files

.PARAMETER BuildType
    CMake build type: Debug, Release

.PARAMETER BuildSharedLibs
    ON for shared libraries, OFF for static

.PARAMETER Subdivide
    If ON, install each library into its own subdirectory

.PARAMETER LogLevel
    CMake log level: ERROR, WARNING, NOTICE, STATUS, VERBOSE, DEBUG, TRACE

.PARAMETER Generator
    CMake generator to use (e.g., "Visual Studio 17 2022", "Ninja")

.PARAMETER ExtraBuildFlags
    Extra flags to pass to the build tool (e.g., "/m:16 /v:q" for MSBuild)

.PARAMETER ExtraCMakeArgs
    Additional CMake arguments as an array

.EXAMPLE
    .\build_deps.ps1 -Platform windows -SourceDir ".\third-party" -BuildDir ".\build" -InstallPrefix ".\install" -BuildType Release -BuildSharedLibs OFF
#>

[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet("windows", "android")]
    [string]$Platform,

    [Parameter(Mandatory = $true)]
    [string]$SourceDir,

    [Parameter(Mandatory = $true)]
    [string]$BuildDir,

    [Parameter(Mandatory = $true)]
    [string]$InstallPrefix,

    [Parameter(Mandatory = $true)]
    [ValidateSet("Debug", "Release")]
    [string]$BuildType,

    [Parameter(Mandatory = $true)]
    [ValidateSet("ON", "OFF")]
    [string]$BuildSharedLibs,

    [ValidateSet("ON", "OFF")]
    [string]$Subdivide = "OFF",

    [ValidateSet("ERROR", "WARNING", "NOTICE", "STATUS", "VERBOSE", "DEBUG", "TRACE")]
    [string]$LogLevel = "ERROR",

    [string]$Generator = "",

    [string]$ExtraBuildFlags = "",

    [string[]]$ExtraCMakeArgs = @()
)

$ErrorActionPreference = "Stop"

# Resolve paths to absolute
$SourceDir = (Resolve-Path $SourceDir).Path
if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir -Force | Out-Null
}
$BuildDir = (Resolve-Path $BuildDir).Path
if (-not (Test-Path $InstallPrefix)) {
    New-Item -ItemType Directory -Path $InstallPrefix -Force | Out-Null
}
$InstallPrefix = (Resolve-Path $InstallPrefix).Path

Write-Host ""
Write-Host "Building dependencies for platform: $Platform" -ForegroundColor Cyan
Write-Host "Subdivision mode: $Subdivide"
if ($Subdivide -eq "ON") {
    Write-Host "Libraries will be installed to: $InstallPrefix\[library_name]\"
} else {
    Write-Host "Libraries will be installed to: $InstallPrefix\"
}
Write-Host ""

# Read dependency lists
$dependenciesAllPlatforms = Join-Path $SourceDir "dependencies_allplatforms.txt"
$dependenciesPlatform = Join-Path $SourceDir "dependencies_$Platform.txt"

$dependencies = @()

if (Test-Path $dependenciesAllPlatforms) {
    $dependencies += Get-Content $dependenciesAllPlatforms | Where-Object { $_ -and $_ -notmatch "^\s*#" }
}

if (Test-Path $dependenciesPlatform) {
    $dependencies += Get-Content $dependenciesPlatform | Where-Object { $_ -and $_ -notmatch "^\s*#" }
}

if ($dependencies.Count -eq 0) {
    Write-Host "ERROR: No dependencies found in dependency files." -ForegroundColor Red
    exit 1
}

Write-Host "Dependencies to build: $($dependencies -join ', ')" -ForegroundColor Yellow
Write-Host ""

# Build generator flag
$generatorFlag = @()
if ($Generator) {
    $generatorFlag = @("-G", $Generator)
    Write-Host "Using CMake generator: $Generator"
}

# Build suppression flags for quiet builds
$suppressWarningsFlags = @()
$cmakeWarnFlags = @()
if ($LogLevel -eq "ERROR") {
    if ($Platform -eq "windows" -and $Generator -notmatch "Ninja") {
        $suppressWarningsFlags = @("-DCMAKE_C_FLAGS=/w", "-DCMAKE_CXX_FLAGS=/w")
    }
    $cmakeWarnFlags = @("--no-warn-unused-cli")
}

# Track previously built libraries for CMAKE_PREFIX_PATH in subdivide mode
$previousLibsPath = $InstallPrefix

# Track build failures
$failedDeps = @()

# Process each dependency
foreach ($dep in $dependencies) {
    $dep = $dep.Trim()
    if (-not $dep) { continue }

    Write-Host ""
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "Building: $dep" -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host ""

    $depBuildDir = Join-Path $BuildDir $dep

    if ($Subdivide -eq "ON") {
        # Per-library subdivision mode
        $libInstallPrefix = Join-Path $InstallPrefix $dep
        $libFindRootPath = $previousLibsPath
        $libPrefixPath = $previousLibsPath

        Write-Host "Installing to: $libInstallPrefix"

        # Build cmake args with modified paths
        $cmakeArgs = @(
            "--log-level=$LogLevel"
        )
        $cmakeArgs += $cmakeWarnFlags
        $cmakeArgs += $generatorFlag
        $cmakeArgs += @(
            "-S", $SourceDir,
            "-B", $depBuildDir,
            "-DINCLUDED_DEP_NAME=$dep",
            "-DCMAKE_INSTALL_PREFIX=$libInstallPrefix",
            "-DCMAKE_BUILD_TYPE=$BuildType",
            "-DBUILD_SHARED_LIBS=$BuildSharedLibs",
            "-DCMAKE_FIND_ROOT_PATH=$libFindRootPath",
            "-DCMAKE_PREFIX_PATH=$libPrefixPath"
        )
        $cmakeArgs += $suppressWarningsFlags
        $cmakeArgs += $ExtraCMakeArgs

        # Configure
        Write-Host "Configuring $dep..." -ForegroundColor Yellow
        & cmake $cmakeArgs
        if ($LASTEXITCODE -ne 0) {
            Write-Host "ERROR: CMake configure failed for $dep" -ForegroundColor Red
            $failedDeps += $dep
            continue
        }

        # Build and install
        Write-Host "Building $dep..." -ForegroundColor Yellow
        $buildArgs = @("--build", $depBuildDir, "--config", $BuildType, "--target", "install")
        if ($ExtraBuildFlags) {
            $buildArgs += @("--")
            $buildArgs += $ExtraBuildFlags -split " "
        }

        & cmake $buildArgs
        if ($LASTEXITCODE -ne 0) {
            Write-Host "ERROR: Build failed for $dep" -ForegroundColor Red
            $failedDeps += $dep
            continue
        }

        # Add this library to the path for future libraries
        if (Test-Path $libInstallPrefix) {
            $previousLibsPath = "$previousLibsPath;$libInstallPrefix"
        }
    } else {
        # Flat structure mode (default)
        $cmakeArgs = @(
            "--log-level=$LogLevel"
        )
        $cmakeArgs += $cmakeWarnFlags
        $cmakeArgs += $generatorFlag
        $cmakeArgs += @(
            "-S", $SourceDir,
            "-B", $depBuildDir,
            "-DINCLUDED_DEP_NAME=$dep",
            "-DCMAKE_INSTALL_PREFIX=$InstallPrefix",
            "-DCMAKE_BUILD_TYPE=$BuildType",
            "-DBUILD_SHARED_LIBS=$BuildSharedLibs",
            "-DCMAKE_FIND_ROOT_PATH=$InstallPrefix",
            "-DCMAKE_PREFIX_PATH=$InstallPrefix"
        )
        $cmakeArgs += $suppressWarningsFlags
        $cmakeArgs += $ExtraCMakeArgs

        # Configure
        Write-Host "Configuring $dep..." -ForegroundColor Yellow
        & cmake $cmakeArgs
        if ($LASTEXITCODE -ne 0) {
            Write-Host "ERROR: CMake configure failed for $dep" -ForegroundColor Red
            $failedDeps += $dep
            continue
        }

        # Build and install
        Write-Host "Building $dep..." -ForegroundColor Yellow
        $buildArgs = @("--build", $depBuildDir, "--config", $BuildType, "--target", "install")
        if ($ExtraBuildFlags) {
            $buildArgs += @("--")
            $buildArgs += $ExtraBuildFlags -split " "
        }

        & cmake $buildArgs
        if ($LASTEXITCODE -ne 0) {
            Write-Host "ERROR: Build failed for $dep" -ForegroundColor Red
            $failedDeps += $dep
            continue
        }
    }

    Write-Host "Successfully built: $dep" -ForegroundColor Green
}

Write-Host ""

if ($failedDeps.Count -eq 0) {
    Write-Host "All dependencies built successfully!" -ForegroundColor Green
    exit 0
} else {
    Write-Host "Some dependencies failed to build:" -ForegroundColor Red
    foreach ($dep in $failedDeps) {
        Write-Host "  - $dep" -ForegroundColor Red
    }
    exit 1
}
