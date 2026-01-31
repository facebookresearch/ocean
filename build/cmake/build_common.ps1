# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

<#
.SYNOPSIS
    Common helper functions for Ocean build scripts.

.DESCRIPTION
    This script provides shared utility functions for Ocean build scripts.
    It should be dot-sourced by other build scripts, not executed directly.

    Usage: . "$PSScriptRoot\build_common.ps1"
#>

# Check if CMake is available and meets minimum version requirements
function Test-CMake {
    param(
        [string]$MinVersion = "3.25"
    )

    $cmake = Get-Command cmake -ErrorAction SilentlyContinue
    if (-not $cmake) {
        Write-Host "ERROR: CMake is not installed or not in your PATH." -ForegroundColor Red
        Write-Host ""
        Write-Host "Please install CMake before running this script:" -ForegroundColor Yellow
        Write-Host "  Download from: https://cmake.org/download/"
        Write-Host "  Or use winget: winget install Kitware.CMake"
        Write-Host "  Or use choco:  choco install cmake"
        return $false
    }

    $versionOutput = cmake --version | Select-Object -First 1
    $version = $versionOutput -replace "cmake version ", ""
    $parts = $version.Split(".")
    $major = [int]$parts[0]
    $minor = [int]$parts[1]

    $minParts = $MinVersion.Split(".")
    $minMajor = [int]$minParts[0]
    $minMinor = [int]$minParts[1]

    if ($major -lt $minMajor -or ($major -eq $minMajor -and $minor -lt $minMinor)) {
        Write-Host "ERROR: CMake $MinVersion or later is required. Current version: $version" -ForegroundColor Red
        return $false
    }

    return $true
}

# Check if Ninja is available
function Test-Ninja {
    $ninja = Get-Command ninja -ErrorAction SilentlyContinue
    if (-not $ninja) {
        Write-Host "ERROR: Ninja is not installed or not in your PATH." -ForegroundColor Red
        Write-Host ""
        Write-Host "Please install Ninja before running this script:" -ForegroundColor Yellow
        Write-Host "  Download from: https://ninja-build.org/"
        Write-Host "  Or use winget: winget install Ninja-build.Ninja"
        Write-Host "  Or use choco:  choco install ninja"
        return $false
    }
    return $true
}

# Check Android build prerequisites
function Test-AndroidPrerequisites {
    $hasErrors = $false

    if (-not $env:ANDROID_HOME) {
        Write-Host "ERROR: ANDROID_HOME environment variable is not set." -ForegroundColor Red
        Write-Host "  Set it to the location of your Android SDK installation." -ForegroundColor Yellow
        Write-Host "  Example: `$env:ANDROID_HOME = 'C:\Android\sdk'" -ForegroundColor Yellow
        $hasErrors = $true
    } elseif (-not (Test-Path $env:ANDROID_HOME)) {
        Write-Host "ERROR: ANDROID_HOME directory does not exist: $env:ANDROID_HOME" -ForegroundColor Red
        $hasErrors = $true
    }

    if (-not $env:ANDROID_NDK) {
        Write-Host "ERROR: ANDROID_NDK environment variable is not set." -ForegroundColor Red
        Write-Host "  Set it to the location of your Android NDK installation." -ForegroundColor Yellow
        Write-Host "  Example: `$env:ANDROID_NDK = 'C:\Android\ndk\25.2.9519653'" -ForegroundColor Yellow
        $hasErrors = $true
    } elseif (-not (Test-Path $env:ANDROID_NDK)) {
        Write-Host "ERROR: ANDROID_NDK directory does not exist: $env:ANDROID_NDK" -ForegroundColor Red
        $hasErrors = $true
    } else {
        $toolchain = Join-Path $env:ANDROID_NDK "build\cmake\android.toolchain.cmake"
        if (-not (Test-Path $toolchain)) {
            Write-Host "ERROR: Android toolchain not found in NDK: $env:ANDROID_NDK" -ForegroundColor Red
            $hasErrors = $true
        }
    }

    if (-not $env:JAVA_HOME) {
        Write-Host "ERROR: JAVA_HOME environment variable is not set." -ForegroundColor Red
        Write-Host "  Set it to the location of your Java installation." -ForegroundColor Yellow
        $hasErrors = $true
    } elseif (-not (Test-Path $env:JAVA_HOME)) {
        Write-Host "ERROR: JAVA_HOME directory does not exist: $env:JAVA_HOME" -ForegroundColor Red
        $hasErrors = $true
    }

    return -not $hasErrors
}

# Detect Visual Studio toolset version (vc142, vc143, vc145, etc.)
# If a generator is specified (e.g., "Visual Studio 17 2022"), extract version from it
# Otherwise, use vswhere to detect the latest installed version
function Get-VSToolsetVersion {
    param(
        [string]$Generator = ""
    )

    # If generator is specified, try to extract VS version from it
    # Format: "Visual Studio XX YYYY" where XX is the major version
    if ($Generator -match "Visual Studio (\d+)") {
        $majorVersion = [int]$Matches[1]

        # Map VS major version to toolset version
        # VS 2019 (16.x) = vc142, VS 2022 (17.x) = vc143, VS 2026 (18.x) = vc145
        if ($majorVersion -ge 18) {
            $toolsetNum = $majorVersion + 127
        } else {
            $toolsetNum = $majorVersion + 126
        }
        return "vc$toolsetNum"
    }

    # Fall back to vswhere detection if no generator specified
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vswhere)) {
        $vswhere = "${env:ProgramFiles}\Microsoft Visual Studio\Installer\vswhere.exe"
    }
    if (-not (Test-Path $vswhere)) {
        return $null
    }

    $vsVersion = & $vswhere -latest -property installationVersion 2>$null
    if (-not $vsVersion) {
        return $null
    }

    # Extract major version (16=VS2019, 17=VS2022, 18=VS2026)
    $majorVersion = [int]($vsVersion.Split(".")[0])

    # Map VS major version to toolset version
    # VS 2015 (14.x) = vc140, VS 2017 (15.x) = vc141, VS 2019 (16.x) = vc142
    # VS 2022 (17.x) = vc143, VS 2026 (18.x) = vc145
    if ($majorVersion -ge 18) {
        $toolsetNum = $majorVersion + 127
    } else {
        $toolsetNum = $majorVersion + 126
    }
    return "vc$toolsetNum"
}

# Get default architecture based on the current system
function Get-DefaultArchitecture {
    if ($env:PROCESSOR_ARCHITECTURE -eq "ARM64") {
        return "arm64"
    } elseif ($env:PROCESSOR_ARCHITECTURE -eq "AMD64") {
        return "x64"
    } elseif ($env:PROCESSOR_ARCHITECTURE -eq "x86") {
        return "x86"
    } else {
        return $env:PROCESSOR_ARCHITECTURE.ToLower()
    }
}

# Map Android ABI to architecture name
function Get-AndroidArchFromABI {
    param(
        [string]$ABI
    )

    switch ($ABI) {
        "arm64-v8a" { return "arm64" }
        "armeabi-v7a" { return "arm" }
        "x86_64" { return "x64" }
        "x86" { return "x86" }
        default { return $ABI }
    }
}

# Construct the build/install directory base name for a given configuration
function Get-ConfigDirectoryName {
    param(
        [string]$Platform,
        [string]$Arch,
        [string]$LinkType,
        [string]$BuildConfig,
        [string]$VSToolset = ""
    )

    $configLower = $BuildConfig.ToLower()
    $linkLower = $LinkType.ToLower()

    if ($Platform -eq "windows" -and $VSToolset) {
        return "$Platform\${Arch}_${VSToolset}_${linkLower}_${configLower}"
    } else {
        return "$Platform\${Arch}_${linkLower}_${configLower}"
    }
}

# Write a section header to the console
function Write-BuildHeader {
    param(
        [string]$Message
    )

    Write-Host ""
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host $Message -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host ""
}

# Write build configuration summary
function Write-BuildSummary {
    param(
        [string]$Platform,
        [string]$Arch,
        [string]$Config,
        [string]$Link,
        [string]$BuildDir,
        [string]$InstallDir
    )

    Write-Host "BUILD_TYPE           $Config"
    Write-Host "BUILD_SHARED_LIBS    $(if ($Link -eq 'shared') { 'ON' } else { 'OFF' })"
    Write-Host "BUILD_DIRECTORY      $BuildDir"
    Write-Host "INSTALL_DIRECTORY    $InstallDir"
}

# Check if Windows Long Path support is enabled
# Returns $true if enabled, $false otherwise
# Displays a warning if not enabled, or a confirmation if enabled
function Test-LongPathSupport {
    try {
        $regPath = "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem"
        $value = Get-ItemProperty -Path $regPath -Name "LongPathsEnabled" -ErrorAction SilentlyContinue

        if ($value -and $value.LongPathsEnabled -eq 1) {
            Write-Host "Long Path support is enabled." -ForegroundColor Green
            return $true
        }
    } catch {
        # Registry access failed, assume not enabled
    }

    Write-Host ""
    Write-Host "WARNING: Windows Long Path support is not enabled." -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Build paths may exceed the 260 character limit (MAX_PATH), causing failures." -ForegroundColor Yellow
    Write-Host ""
    Write-Host "To enable Long Path support, run this command as Administrator:" -ForegroundColor Cyan
    Write-Host "  Set-ItemProperty -Path 'HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem' -Name 'LongPathsEnabled' -Value 1" -ForegroundColor White
    Write-Host ""
    Write-Host "Alternatively, use the Group Policy Editor:" -ForegroundColor Cyan
    Write-Host "  Computer Configuration > Administrative Templates > System > Filesystem" -ForegroundColor White
    Write-Host "  Enable: 'Enable Win32 long paths'" -ForegroundColor White
    Write-Host ""
    Write-Host "A system restart may be required after enabling this setting." -ForegroundColor Yellow
    Write-Host ""

    return $false
}
