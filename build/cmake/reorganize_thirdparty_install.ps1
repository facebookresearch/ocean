# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

<#
.SYNOPSIS
    Reorganize third-party library installations from CMake --subdivide structure
    to a version-first, platform-organized structure.

.DESCRIPTION
    This script reorganizes third-party library installations from the CMake
    subdivision output structure to a more organized version-first structure.

    Supported platforms: Windows, Android (built on Windows)

    Input structure (--subdivide output):
      install_3p\
      +-- windows\
      |   +-- x64_vc143_static_debug\
      |   |   +-- zlib\
      |   |   |   +-- include\
      |   |   |   +-- lib\
      |   |   +-- eigen\
      |   |       +-- include\
      +-- android\
          +-- arm64-v8a_static_debug\
              +-- zlib\
                  +-- include\
                  +-- lib\

    Output structure (reorganized):
      reorganized_3p\
      +-- zlib\
      |   +-- 1.x.x\
      |       +-- include\
      |       +-- lib\
      |           +-- win_x64_vc143_static_debug\
      |           +-- android_arm64_cl18_static_debug\
      +-- eigen\
          +-- 3.x.x\
              +-- include\

.PARAMETER InputDir
    The directory containing --subdivide output (e.g., install_3p with
    subdirectories like windows\x64_vc143_static_debug). Required.

.PARAMETER OutputDir
    The output directory for reorganized structure. Default: .\reorganized_3p

.PARAMETER VersionMapFile
    Optional file mapping library names to versions. Each line should be
    in the format: library_name=version (e.g., zlib=1.3.1)

.EXAMPLE
    .\reorganize_thirdparty_install.ps1 -InputDir .\ocean_install_thirdparty

.EXAMPLE
    .\reorganize_thirdparty_install.ps1 -InputDir .\install_3p -OutputDir .\reorganized_3p

.EXAMPLE
    .\reorganize_thirdparty_install.ps1 -InputDir .\install_3p -VersionMapFile .\versions.txt
#>

[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$InputDir,

    [string]$OutputDir = ".\reorganized_3p",

    [string]$VersionMapFile = ""
)

$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

# Validate input directory exists
if (-not (Test-Path $InputDir)) {
    Write-Host "ERROR: Input directory does not exist: $InputDir" -ForegroundColor Red
    exit 1
}

$InputDir = (Resolve-Path $InputDir).Path

# Create output directory
if (-not (Test-Path $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
}
$OutputDir = (Resolve-Path $OutputDir).Path

Write-Host "Reorganizing third-party libraries..." -ForegroundColor Cyan
Write-Host ""
Write-Host "Input directory:  $InputDir"
Write-Host "Output directory: $OutputDir"
Write-Host ""

# Load version map if provided
$versionMap = @{}
if ($VersionMapFile -and (Test-Path $VersionMapFile)) {
    Get-Content $VersionMapFile | ForEach-Object {
        if ($_ -match "^([^=]+)=(.+)$") {
            $versionMap[$Matches[1].Trim()] = $Matches[2].Trim()
        }
    }
    Write-Host "Loaded version map with $($versionMap.Count) entries"
}

# Function to map platform directory names to Ocean-native platform names
# Naming convention: {platform}_{arch}_{compiler}{version}_{linktype}[_debug]
function Get-MappedPlatformName {
    param(
        [string]$PlatformDir,
        [string]$ConfigDir
    )

    # Parse the config directory name
    # Windows format: {arch}_{vs_toolset}_{linktype}_{config} or {arch}_{linktype}_{config}
    # Android format: {abi}_{linktype}_{config}

    switch ($PlatformDir) {
        "windows" {
            # Try to parse: x64_vc143_static_debug or x64_static_debug
            if ($ConfigDir -match "^(x64|arm64|x86)_vc(\d+)_(static|shared)_(debug|release)$") {
                $arch = $Matches[1]
                $toolset = "vc$($Matches[2])"
                $linkType = $Matches[3]
                $config = $Matches[4]

                if ($config -eq "debug") {
                    return "win_${arch}_${toolset}_${linkType}_debug"
                } else {
                    return "win_${arch}_${toolset}_${linkType}"
                }
            } elseif ($ConfigDir -match "^(x64|arm64|x86)_(static|shared)_(debug|release)$") {
                $arch = $Matches[1]
                $linkType = $Matches[2]
                $config = $Matches[3]
                $toolset = "vc143"  # Default

                if ($config -eq "debug") {
                    return "win_${arch}_${toolset}_${linkType}_debug"
                } else {
                    return "win_${arch}_${toolset}_${linkType}"
                }
            }
        }
        "android" {
            # Parse: arm64-v8a_static_debug
            if ($ConfigDir -match "^(arm64-v8a|armeabi-v7a|x86_64|x86)_(static|shared)_(debug|release)$") {
                $abi = $Matches[1]
                $linkType = $Matches[2]
                $config = $Matches[3]

                # Map ABI to architecture
                $arch = switch ($abi) {
                    "arm64-v8a" { "arm64" }
                    "armeabi-v7a" { "arm" }
                    "x86_64" { "x64" }
                    "x86" { "x86" }
                    default { $abi }
                }

                $compiler = "cl18"  # Default to Clang 18 (NDK r26+)

                if ($config -eq "debug") {
                    return "android_${arch}_${compiler}_${linkType}_debug"
                } else {
                    return "android_${arch}_${compiler}_${linkType}"
                }
            }
        }
        "macos" {
            if ($ConfigDir -match "^(x64|arm64)_(static|shared)_(debug|release)$") {
                $arch = $Matches[1]
                $linkType = $Matches[2]
                $config = $Matches[3]
                $compiler = "xc15"  # Default to Xcode 15

                if ($config -eq "debug") {
                    return "osx_${arch}_${compiler}_${linkType}_debug"
                } else {
                    return "osx_${arch}_${compiler}_${linkType}"
                }
            }
        }
        "linux" {
            if ($ConfigDir -match "^(x64|arm64)_(static|shared)_(debug|release)$") {
                $arch = $Matches[1]
                $linkType = $Matches[2]
                $config = $Matches[3]
                $compiler = "gcc13"  # Default to GCC 13

                if ($config -eq "debug") {
                    return "linux_${arch}_${compiler}_${linkType}_debug"
                } else {
                    return "linux_${arch}_${compiler}_${linkType}"
                }
            }
        }
        "ios" {
            if ($ConfigDir -match "^(OS64|SIMULATOR64|SIMULATORARM64)_(static|shared)_(debug|release)$") {
                $toolchain = $Matches[1]
                $linkType = $Matches[2]
                $config = $Matches[3]

                # Map toolchain to architecture
                $arch = switch ($toolchain) {
                    "OS64" { "arm64" }
                    "SIMULATOR64" { "x64" }
                    "SIMULATORARM64" { "arm64" }
                    default { $toolchain }
                }

                $compiler = "xc15"  # Default to Xcode 15

                if ($config -eq "debug") {
                    return "ios_${arch}_${compiler}_${linkType}_debug"
                } else {
                    return "ios_${arch}_${compiler}_${linkType}"
                }
            }
        }
    }

    # Return null if no match
    return $null
}

# Function to get library version
function Get-LibraryVersion {
    param(
        [string]$LibDir,
        [string]$LibName
    )

    # First priority: Check for version.txt in the library directory
    $versionFile = Join-Path $LibDir "version.txt"
    if (Test-Path $versionFile) {
        $version = (Get-Content $versionFile -First 1).Trim()
        if ($version) {
            return $version
        }
    }

    # Second priority: Use version map
    if ($versionMap.ContainsKey($LibName)) {
        return $versionMap[$LibName]
    }

    # Default
    return "current"
}

# Track processed headers (to avoid copying headers multiple times)
$processedHeaders = @{}

# Process each platform directory
$platformDirs = Get-ChildItem -Path $InputDir -Directory -ErrorAction SilentlyContinue
foreach ($platformDir in $platformDirs) {
    $platformName = $platformDir.Name
    Write-Host "Processing platform: $platformName" -ForegroundColor Yellow

    # Process each config directory within the platform
    $configDirs = Get-ChildItem -Path $platformDir.FullName -Directory -ErrorAction SilentlyContinue
    foreach ($configDir in $configDirs) {
        $configName = $configDir.Name

        # Map to Ocean-native platform name
        $mappedPlatform = Get-MappedPlatformName -PlatformDir $platformName -ConfigDir $configName
        if (-not $mappedPlatform) {
            Write-Host "  WARNING: Skipping invalid config directory: $configName" -ForegroundColor Yellow
            continue
        }

        Write-Host "  Config: $configName -> $mappedPlatform" -ForegroundColor Gray

        # Process each library in this config directory
        $libDirs = Get-ChildItem -Path $configDir.FullName -Directory -ErrorAction SilentlyContinue
        foreach ($libDir in $libDirs) {
            $libName = $libDir.Name
            Write-Host "    Processing library: $libName" -ForegroundColor Gray

            # Get version
            $libVersion = Get-LibraryVersion -LibDir $libDir.FullName -LibName $libName
            Write-Host "      Version: $libVersion" -ForegroundColor DarkGray

            # Create output structure
            $outputLibDir = Join-Path $OutputDir "$libName\$libVersion"
            if (-not (Test-Path $outputLibDir)) {
                New-Item -ItemType Directory -Path $outputLibDir -Force | Out-Null
            }

            # Copy headers (only once per library)
            $headerKey = "$libName|$libVersion"
            $includeDir = Join-Path $libDir.FullName "include"
            if ((Test-Path $includeDir) -and -not $processedHeaders.ContainsKey($headerKey)) {
                Write-Host "      Copying headers..." -ForegroundColor DarkGray
                $outputIncludeDir = Join-Path $outputLibDir "include"
                if (-not (Test-Path $outputIncludeDir)) {
                    New-Item -ItemType Directory -Path $outputIncludeDir -Force | Out-Null
                }

                # Copy headers, excluding cmake files
                Get-ChildItem -Path $includeDir -Recurse -File |
                    Where-Object { $_.Extension -ne ".cmake" } |
                    ForEach-Object {
                        $relativePath = $_.FullName.Substring($includeDir.Length + 1)
                        $destPath = Join-Path $outputIncludeDir $relativePath
                        $destDir = Split-Path -Parent $destPath
                        if (-not (Test-Path $destDir)) {
                            New-Item -ItemType Directory -Path $destDir -Force | Out-Null
                        }
                        Copy-Item -Path $_.FullName -Destination $destPath -Force
                    }

                $processedHeaders[$headerKey] = $true
            } elseif ($processedHeaders.ContainsKey($headerKey)) {
                Write-Host "      Headers already copied" -ForegroundColor DarkGray
            }

            # Copy libraries to platform-specific subdirectory
            $libSubDir = Join-Path $libDir.FullName "lib"
            if (Test-Path $libSubDir) {
                Write-Host "      Copying libraries to lib\$mappedPlatform\..." -ForegroundColor DarkGray
                $outputPlatformLib = Join-Path $outputLibDir "lib\$mappedPlatform"
                if (-not (Test-Path $outputPlatformLib)) {
                    New-Item -ItemType Directory -Path $outputPlatformLib -Force | Out-Null
                }

                # Copy library files, excluding cmake/pkgconfig files
                Get-ChildItem -Path $libSubDir -Recurse -File |
                    Where-Object {
                        $_.Extension -notin @(".cmake", ".pc") -and
                        $_.Directory.Name -notin @("cmake", "pkgconfig")
                    } |
                    ForEach-Object {
                        $relativePath = $_.FullName.Substring($libSubDir.Length + 1)
                        $destPath = Join-Path $outputPlatformLib $relativePath
                        $destDir = Split-Path -Parent $destPath
                        if (-not (Test-Path $destDir)) {
                            New-Item -ItemType Directory -Path $destDir -Force | Out-Null
                        }
                        Copy-Item -Path $_.FullName -Destination $destPath -Force
                    }
            }

            # Copy bin directory if it exists (for DLLs on Windows)
            $binSubDir = Join-Path $libDir.FullName "bin"
            if (Test-Path $binSubDir) {
                Write-Host "      Copying binaries to bin\$mappedPlatform\..." -ForegroundColor DarkGray
                $outputPlatformBin = Join-Path $outputLibDir "bin\$mappedPlatform"
                if (-not (Test-Path $outputPlatformBin)) {
                    New-Item -ItemType Directory -Path $outputPlatformBin -Force | Out-Null
                }

                Get-ChildItem -Path $binSubDir -Recurse -File | ForEach-Object {
                    $relativePath = $_.FullName.Substring($binSubDir.Length + 1)
                    $destPath = Join-Path $outputPlatformBin $relativePath
                    $destDir = Split-Path -Parent $destPath
                    if (-not (Test-Path $destDir)) {
                        New-Item -ItemType Directory -Path $destDir -Force | Out-Null
                    }
                    Copy-Item -Path $_.FullName -Destination $destPath -Force
                }
            }
        }
    }
    Write-Host ""
}

Write-Host "Reorganization complete!" -ForegroundColor Green
Write-Host ""
Write-Host "Output directory: $OutputDir" -ForegroundColor Cyan
exit 0
