# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

<#
.SYNOPSIS
    Unified build script for Ocean using CMake Presets.

.DESCRIPTION
    Supports native Windows builds and cross-compilation for Android.
    Uses CMake presets for configuration management.

.PARAMETER Platform
    Target platform: win, android. Default: win

.PARAMETER Config
    Build configuration(s): debug, release, or both (comma-separated). Default: release

.PARAMETER Link
    Linking type(s): static, shared, or both (comma-separated). Default: static

.PARAMETER Arch
    Architecture(s): x64, arm64 (for Windows), arm64, arm32, x64 (for Android).
    Comma-separated for multiple. Default: auto-detect

.PARAMETER Build
    Build directory. Default: .\bin\cmake\tmp

.PARAMETER Install
    Install directory. Default: .\bin\cmake

.PARAMETER ThirdParty
    Path to third-party libraries. Default: .\bin\cmake\3rdparty

.PARAMETER Quest
    Build for Meta Quest (Android ARM64 with Quest extensions)

.PARAMETER Sdk
    Android SDK version (e.g., android-32). Default: android-32

.PARAMETER Generator
    CMake generator to use. For Windows, you can specify a Visual Studio version:
    "Visual Studio 17 2022", "Visual Studio 16 2019", etc.
    By default, CMake auto-detects the newest available Visual Studio.

.PARAMETER Minimal
    Enable minimal build (base, cv, math only)

.PARAMETER LogLevel
    CMake log level: ERROR, WARNING, NOTICE, STATUS, VERBOSE, DEBUG, TRACE. Default: ERROR

.PARAMETER Sequential
    Build configurations sequentially instead of in parallel

.PARAMETER ConfigureOnly
    Only run CMake configure step, skip build

.PARAMETER ListPresets
    List all available presets and exit

.EXAMPLE
    .\build_ocean.ps1
    Build release static for Windows x64

.EXAMPLE
    .\build_ocean.ps1 -Platform android -Arch arm64
    Cross-compile for Android ARM64

.EXAMPLE
    .\build_ocean.ps1 -Platform android -Arch arm64,arm32,x64
    Cross-compile for Android with multiple ABIs

.EXAMPLE
    .\build_ocean.ps1 -Quest
    Build for Meta Quest

.EXAMPLE
    .\build_ocean.ps1 -Config debug,release
    Build both debug and release configurations

.EXAMPLE
    .\build_ocean.ps1 -Build C:\ocean_build -Install C:\ocean_install
    Build with custom directories

.EXAMPLE
    .\build_ocean.ps1 -Generator "Visual Studio 16 2019"
    Build using Visual Studio 2019 instead of auto-detected version

.EXAMPLE
    .\build_ocean.ps1 -Minimal -LogLevel STATUS
    Minimal build with verbose CMake output
#>

[CmdletBinding()]
param(
[ValidateSet("win", "android")]
    [string]$Platform = "win",

    [string]$Config = "debug,release",

    [string]$Link = "static",

    [string]$Arch = "",

    [string]$Build = ".\bin\cmake\tmp",

    [string]$Install = ".\bin\cmake",

    [string]$ThirdParty = ".\bin\cmake\3rdparty",

    [switch]$Quest,

    [string]$Sdk = "android-32",

    [string]$Generator = "",

    [switch]$Minimal,

    [ValidateSet("ERROR", "WARNING", "NOTICE", "STATUS", "VERBOSE", "DEBUG", "TRACE")]
    [string]$LogLevel = "ERROR",

    [switch]$Sequential,

    [switch]$ConfigureOnly,

    [switch]$ListPresets
)

$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$OceanSourceDir = (Resolve-Path (Join-Path $ScriptDir "..\..")).Path

# Dot-source common functions
. (Join-Path $ScriptDir "build_common.ps1")

# List available presets
function Show-Presets {
    Set-Location $OceanSourceDir
    Write-Host "Available CMake presets:" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "=== Configure Presets ===" -ForegroundColor Green
    cmake --list-presets=configure 2>$null
    Write-Host ""
    Write-Host "=== Workflow Presets ===" -ForegroundColor Green
    cmake --list-presets=workflow 2>$null
    Write-Host ""
    Write-Host "To use a preset directly:" -ForegroundColor Yellow
    Write-Host "  cmake --preset <preset-name>"
    Write-Host "  cmake --build --preset <preset-name>"
}

# Get default architecture
function Get-DefaultArch {
    param([string]$TargetPlatform)

    switch ($TargetPlatform) {
        "windows" {
            if ($env:PROCESSOR_ARCHITECTURE -eq "ARM64") { return "arm64" }
            else { return "x64" }
        }
        "android" { return "arm64" }
        default { return "x64" }
    }
}

# Validate architecture
function Test-Arch {
    param([string]$TargetPlatform, [string]$Architecture)

    $valid = switch ($TargetPlatform) {
        "windows" { @("x64", "arm64") }
        "android" { @("arm64", "arm32", "x64") }
        default { @("x64") }
    }

    if ($Architecture -notin $valid) {
        Write-Error "ERROR: Invalid architecture '$Architecture' for $TargetPlatform. Valid: $($valid -join ', ')"
        exit 1
    }
}

# Build a single configuration
function Invoke-Build {
    param(
        [string]$TargetPlatform,
        [string]$Architecture,
        [string]$LinkType,
        [string]$BuildConfig,
        [string]$BuildDir,
        [string]$InstallDir,
        [string]$ThirdPartyDir,
        [string]$AndroidSdk,
        [string]$CmakeLogLevel,
        [string]$CmakeGenerator,
        [bool]$IsQuest,
        [bool]$IsMinimal,
        [bool]$IsConfigureOnly
    )

    # Construct preset name
    # Map platform name to preset name (presets use 'windows' not 'win')
    $presetPlatform = if ($TargetPlatform -eq "win") { "windows" } else { $TargetPlatform }

    if ($IsQuest -and $TargetPlatform -eq "android") {
        $presetName = "android-quest-$LinkType-$BuildConfig"
    } else {
        $presetName = "$presetPlatform-$Architecture-$LinkType-$BuildConfig"
    }

    Write-Host ""
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host "Building: $presetName" -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host ""

    # Determine third-party path (include VS toolset for Windows)
    $tpArch = if ($IsQuest -and $TargetPlatform -eq "android") { "arm64" } else { $Architecture }
    if ($TargetPlatform -eq "windows" -and $script:vsToolset) {
        $thirdPartyPath = Join-Path $ThirdPartyDir "$TargetPlatform\${tpArch}${script:vsToolset}_${LinkType}_${BuildConfig}"
    } else {
        $thirdPartyPath = Join-Path $ThirdPartyDir "$TargetPlatform\${tpArch}_${LinkType}_${BuildConfig}"
    }

    if (-not (Test-Path $thirdPartyPath)) {
        Write-Warning "Third-party directory not found: $thirdPartyPath"
        Write-Warning "Build may fail if dependencies are not in system paths."
    }

    # Build CMAKE_PREFIX_PATH
    $cmakePrefixPath = $thirdPartyPath
    if (Test-Path $thirdPartyPath) {
        $subdirs = Get-ChildItem -Path $thirdPartyPath -Directory -ErrorAction SilentlyContinue
        if ($subdirs -and ($subdirs.Name -contains "zlib" -or $subdirs.Name -contains "eigen")) {
            # Per-library subdivision structure
            $cmakePrefixPath = ($subdirs | ForEach-Object { $_.FullName }) -join ";"
        }
    }

    # Determine build and install directories for this configuration
    if ($IsQuest -and $TargetPlatform -eq "android") {
        $configBuildDir = Join-Path $BuildDir "$TargetPlatform\quest_${LinkType}_${BuildConfig}"
        $configInstallDir = Join-Path $InstallDir "$TargetPlatform\quest_${LinkType}_${BuildConfig}"
    } else {
        # Use the global $vsToolset variable for Windows builds
        $configBuildDir = Join-Path $BuildDir "$TargetPlatform\${Architecture}${script:vsToolset}_${LinkType}_${BuildConfig}"
        $configInstallDir = Join-Path $InstallDir "$TargetPlatform\${Architecture}${script:vsToolset}_${LinkType}_${BuildConfig}"
    }

    # Configure
    $configArgs = @(
        "--preset", $presetName,
        "--log-level=$CmakeLogLevel",
        "-DCMAKE_PREFIX_PATH=$cmakePrefixPath",
        "-B", $configBuildDir,
        "-DCMAKE_INSTALL_PREFIX=$configInstallDir"
    )

    # Add generator override if specified
    if ($CmakeGenerator) {
        $configArgs += @("-G", $CmakeGenerator)
    }

    if ($TargetPlatform -eq "android") {
        $configArgs += "-DCMAKE_FIND_ROOT_PATH=$cmakePrefixPath"
        $configArgs += "-DANDROID_PLATFORM=$AndroidSdk"
    }

    if ($IsMinimal) {
        $configArgs += "-DOCEAN_BUILD_MINIMAL=ON"
        $configArgs += "-DOCEAN_BUILD_DEMOS=OFF"
        $configArgs += "-DOCEAN_BUILD_TESTS=OFF"
    }

    Write-Host "Configuring..." -ForegroundColor Yellow
    & cmake $configArgs
    if ($LASTEXITCODE -ne 0) {
        return $false
    }

    if ($IsConfigureOnly) {
        Write-Host "Configure-only mode: skipping build step." -ForegroundColor Yellow
        return $true
    }

    # Build and install
    Write-Host "Building..." -ForegroundColor Yellow

    # Capitalize first letter of config for multi-config generators
    $configCapitalized = (Get-Culture).TextInfo.ToTitleCase($BuildConfig.ToLower())

    $buildArgs = @("--build", $configBuildDir, "--target", "install", "--config", $configCapitalized)

    # Add Visual Studio-specific options
    if ($CmakeLogLevel -eq "ERROR") {
        $buildArgs += @("--", "/m:16", "/v:q")
    } else {
        $buildArgs += @("--", "/m:16")
    }

    & cmake $buildArgs
    return ($LASTEXITCODE -eq 0)
}

# Main script
if (-not (Test-CMake)) {
    exit 1
}

# Check for Long Path support on Windows
if ($Platform -eq "win") {
    Test-LongPathSupport | Out-Null
}

if ($ListPresets) {
    Show-Presets
    exit 0
}

# Handle Quest mode
if ($Quest) {
    $Platform = "android"
}

# Check platform-specific prerequisites
switch ($Platform) {
    "android" {
        if (-not (Test-AndroidPrerequisites)) {
            exit 1
        }
    }
}

# Set default architecture
if (-not $Arch) {
    $Arch = Get-DefaultArch -TargetPlatform $Platform
}

# Parse architectures (support comma-separated list)
$archs = $Arch -split "," | ForEach-Object { $_.Trim().ToLower() }
foreach ($a in $archs) {
    Test-Arch -TargetPlatform $Platform -Architecture $a
}

# Parse configurations
$configs = $Config -split "," | ForEach-Object { $_.Trim().ToLower() }
foreach ($c in $configs) {
    if ($c -notin @("debug", "release")) {
        Write-Error "ERROR: Invalid build configuration: $c. Valid: debug, release"
        exit 1
    }
}

# Parse link types
$linkTypes = $Link -split "," | ForEach-Object { $_.Trim().ToLower() }
foreach ($l in $linkTypes) {
    if ($l -notin @("static", "shared")) {
        Write-Error "ERROR: Invalid linking type: $l. Valid: static, shared"
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

if (Test-Path $ThirdParty) {
    $ThirdParty = (Resolve-Path $ThirdParty).Path
} else {
    Write-Warning "Third-party directory not found: $ThirdParty"
    Write-Warning "Proceeding anyway - build may fail if dependencies are missing."
}

# Change to source directory
Set-Location $OceanSourceDir

# Detect VS toolset version for Windows builds
$vsToolset = ""
if ($Platform -eq "win") {
    $vsToolset = Get-VSToolsetVersion -Generator $Generator
    if ($vsToolset) {
        Write-Host "Detected Visual Studio toolset: $vsToolset" -ForegroundColor Green
        $vsToolset = "_$vsToolset"
    } else {
        Write-Warning "Could not detect Visual Studio toolset version, using default naming."
    }
}

# Display build plan
Write-Host ""
Write-Host "Ocean Build Configuration" -ForegroundColor Cyan
Write-Host "=========================" -ForegroundColor Cyan
Write-Host "  Target platform: $Platform"
Write-Host "  Architecture(s): $Arch"
Write-Host "  Configurations:  $Config"
Write-Host "  Link types:      $Link"
Write-Host "  Build dir:       $Build"
Write-Host "  Install dir:     $Install"
Write-Host "  Third-party:     $ThirdParty"
Write-Host "  Log level:       $LogLevel"
if ($Generator) {
    Write-Host "  Generator:       $Generator" -ForegroundColor Magenta
} else {
    Write-Host "  Generator:       (auto-detect)"
}
if ($Quest) {
    Write-Host "  Quest mode:      enabled" -ForegroundColor Magenta
}
if ($Minimal) {
    Write-Host "  Minimal build:   enabled" -ForegroundColor Magenta
}
if ($Platform -eq "android") {
    Write-Host "  Android SDK:     $Sdk"
}
Write-Host "  Parallel:        $(-not $Sequential)"
Write-Host ""
Write-Host "Builds to run:" -ForegroundColor Yellow
foreach ($a in $archs) {
    foreach ($c in $configs) {
        foreach ($l in $linkTypes) {
            if ($Quest) {
                Write-Host "  - android-quest-$l-$c"
            } else {
                Write-Host "  - $Platform-$a-$l-$c"
            }
        }
    }
}
Write-Host ""

# Track failed builds
$failedBuilds = @()

# Run builds
$totalBuilds = $archs.Count * $configs.Count * $linkTypes.Count
if (-not $Sequential -and $totalBuilds -gt 1) {
    Write-Host "Building configurations in parallel..." -ForegroundColor Green
    Write-Host ""

    $jobs = @()
    foreach ($a in $archs) {
        foreach ($c in $configs) {
            foreach ($l in $linkTypes) {
                $jobs += Start-Job -ScriptBlock {
                    param($scriptPath, $platform, $arch, $linkType, $config, $buildDir, $installDir, $thirdParty, $sdk, $logLevel, $generator, $quest, $minimal, $configOnly, $sourceDir)
                    Set-Location $sourceDir

                    $params = @{
                        Platform = $platform
                        Arch = $arch
                        Link = $linkType
                        Config = $config
                        Build = $buildDir
                        Install = $installDir
                        ThirdParty = $thirdParty
                        Sdk = $sdk
                        LogLevel = $logLevel
                        Sequential = $true
                    }

                    if ($generator) { $params["Generator"] = $generator }
                    if ($quest) { $params["Quest"] = $true }
                    if ($minimal) { $params["Minimal"] = $true }
                    if ($configOnly) { $params["ConfigureOnly"] = $true }

                    & $scriptPath @params
                } -ArgumentList $MyInvocation.MyCommand.Path, $Platform, $a, $l, $c, $Build, $Install, $ThirdParty, $Sdk, $LogLevel, $Generator, $Quest.IsPresent, $Minimal.IsPresent, $ConfigureOnly.IsPresent, $OceanSourceDir
            }
        }
    }

    # Wait for all jobs and collect results
    $jobs | Wait-Job | Out-Null
    foreach ($job in $jobs) {
        $result = Receive-Job -Job $job
        if ($job.State -eq "Failed") {
            $failedBuilds += $job.Name
        }
        Remove-Job -Job $job
    }
} else {
    Write-Host "Building configurations sequentially..." -ForegroundColor Green
    Write-Host ""

    foreach ($a in $archs) {
        foreach ($c in $configs) {
            foreach ($l in $linkTypes) {
                $success = Invoke-Build -TargetPlatform $Platform -Architecture $a -LinkType $l -BuildConfig $c `
                    -BuildDir $Build -InstallDir $Install -ThirdPartyDir $ThirdParty `
                    -AndroidSdk $Sdk -CmakeLogLevel $LogLevel -CmakeGenerator $Generator `
                    -IsQuest $Quest.IsPresent -IsMinimal $Minimal.IsPresent -IsConfigureOnly $ConfigureOnly.IsPresent
                if (-not $success) {
                    if ($Quest) {
                        $failedBuilds += "android-quest-$l-$c"
                    } else {
                        $failedBuilds += "$Platform-$a-$l-$c"
                    }
                }
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
    Write-Host ""
    Write-Host "Install locations:" -ForegroundColor Yellow
    foreach ($a in $archs) {
        foreach ($c in $configs) {
            foreach ($l in $linkTypes) {
                if ($Quest) {
                    Write-Host "  - $Install\$Platform\quest_${l}_${c}"
                } else {
                    Write-Host "  - $Install\$Platform\${a}${vsToolset}_${l}_${c}"
                }
            }
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
