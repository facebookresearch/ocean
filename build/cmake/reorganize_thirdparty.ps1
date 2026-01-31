# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

<#
.SYNOPSIS
    Reorganizes CMake-built third-party libraries into a custom folder structure.

.DESCRIPTION
    This script takes the output from build_thirdparty_windows.ps1 or build_thirdparty_android.ps1
    (with -Subdivide ON) and reorganizes it into a library-first folder structure suitable for
    Visual Studio projects.

    The target structure is:
        {TargetDir}\{library}\{platform}\h\...                    - Headers per library and platform
        {TargetDir}\{library}\{platform}\lib\{config}\...         - Libraries per library, platform, and configuration

    Where {config} follows the pattern:
        Windows: {platform}_{arch}_{vsversion}_{linktype}_{buildtype}
                 Example: win_x64_vc145_static_debug
        Android: {platform}_{arch}_{linktype}_{buildtype}
                 Example: android_arm64_static_release

.PARAMETER Source
    Source directory containing the CMake build output (with Subdivide ON).
    Default: .\bin\cmake\3rdparty

.PARAMETER Target
    Target directory for the reorganized structure.
    Default: ..\3rdparty (relative to ocean root, i.e., xplat\ocean\3rdparty)

.PARAMETER Platform
    Target platform: win, android.
    Default: win

.PARAMETER ABI
    Android ABI(s) to process: arm64-v8a, armeabi-v7a, x86_64, x86 (comma-separated).
    Only used when Platform is android. Architecture names (arm64, arm, x64, x86) are
    derived from these ABIs for directory naming.
    Default: arm64-v8a

.PARAMETER Config
    Build configuration(s) to process: debug, release, or both (comma-separated).
    Default: debug,release

.PARAMETER Link
    Linking type(s) to process: static, shared, or both (comma-separated).
    Default: static

.PARAMETER Clean
    If specified, remove the platform subdirectory in target before reorganizing.

.PARAMETER DryRun
    If specified, show what would be copied without actually copying.

.EXAMPLE
    .\reorganize_thirdparty.ps1
    Reorganize debug and release static libraries for Windows

.EXAMPLE
    .\reorganize_thirdparty.ps1 -Platform android -ABI arm64-v8a
    Reorganize debug and release static libraries for Android arm64

.EXAMPLE
    .\reorganize_thirdparty.ps1 -Platform android -ABI "arm64-v8a,armeabi-v7a"
    Reorganize for multiple Android ABIs

.EXAMPLE
    .\reorganize_thirdparty.ps1 -Config release -DryRun
    Show what would be reorganized for release only

.EXAMPLE
    .\reorganize_thirdparty.ps1 -Clean
    Clean target platform directory and reorganize all configurations
#>

[CmdletBinding()]
param(
    [string]$Source = ".\bin\cmake\3rdparty",
    [string]$Target = "..\3rdparty",

    [ValidateSet("win", "android")]
    [string]$Platform = "win",

    [string]$ABI = "arm64-v8a",
    [string]$Config = "debug,release",
    [string]$Link = "static",
    [switch]$Clean,
    [switch]$DryRun
)

$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$OceanRoot = Split-Path -Parent (Split-Path -Parent $ScriptDir)

# Dot-source common functions
. (Join-Path $ScriptDir "build_common.ps1")

# Resolve source directory
if (-not [System.IO.Path]::IsPathRooted($Source)) {
    $Source = Join-Path $OceanRoot "build\cmake" | Join-Path -ChildPath $Source.TrimStart(".\")
}
if (-not (Test-Path $Source)) {
    Write-Host "ERROR: Source directory does not exist: $Source" -ForegroundColor Red
    if ($Platform -eq "win") {
        Write-Host "Run build_thirdparty_windows.ps1 with -Subdivide ON first." -ForegroundColor Yellow
    } else {
        Write-Host "Run build_thirdparty_android.ps1 with -Subdivide ON first." -ForegroundColor Yellow
    }
    exit 1
}
$Source = (Resolve-Path $Source).Path

# Resolve target directory
if (-not [System.IO.Path]::IsPathRooted($Target)) {
    $Target = Join-Path $OceanRoot $Target.TrimStart("..\").TrimStart(".\")
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

# Platform-specific setup
$sourcePlatformDir = Join-Path $Source $Platform

if ($Platform -eq "win") {
    # Detect architecture for Windows
    $Arch = Get-DefaultArchitecture

    # Detect VS toolset from existing build directories
    $vsToolset = ""
    if (Test-Path $sourcePlatformDir) {
        $existingDirs = Get-ChildItem -Path $sourcePlatformDir -Directory | Select-Object -First 1
        if ($existingDirs) {
            if ($existingDirs.Name -match "^${Arch}_([^_]+)_") {
                $vsToolset = $Matches[1]
            }
        }
    }

    if (-not $vsToolset) {
        Write-Host "Warning: Could not detect VS toolset from existing builds, using default." -ForegroundColor Yellow
        $vsToolset = "vc145"
    }

    $archList = @($Arch)
} else {
    # Parse Android ABIs and convert to architecture names
    $validABIs = @("arm64-v8a", "armeabi-v7a", "x86_64", "x86")
    $abiList = $ABI -split "," | ForEach-Object { $_.Trim() }
    foreach ($a in $abiList) {
        if ($a -notin $validABIs) {
            Write-Host "ERROR: Invalid Android ABI: $a. Valid: $($validABIs -join ', ')" -ForegroundColor Red
            exit 1
        }
    }

    # Convert ABIs to architecture names
    $archList = $abiList | ForEach-Object { Get-AndroidArchFromABI -ABI $_ }

    $vsToolset = ""
}

# Display configuration
Write-Host ""
Write-Host "Third-Party Library Reorganization" -ForegroundColor Cyan
Write-Host "===================================" -ForegroundColor Cyan
Write-Host "  Source:          $Source"
Write-Host "  Target:          $Target"
Write-Host "  Platform:        $Platform"
if ($Platform -eq "win") {
    Write-Host "  Architecture:    $Arch"
    Write-Host "  VS Toolset:      $vsToolset"
} else {
    Write-Host "  ABIs:            $ABI"
    Write-Host "  Architectures:   $($archList -join ', ')"
}
Write-Host "  Configurations:  $Config"
Write-Host "  Link types:      $Link"
if ($DryRun) {
    Write-Host "  Mode:            DRY RUN (no files will be copied)" -ForegroundColor Yellow
}
if ($Clean) {
    Write-Host "  Clean:           Yes" -ForegroundColor Yellow
}
Write-Host ""
Write-Host "Target structure (library-first):" -ForegroundColor Yellow
Write-Host "  {library}\$Platform\h\...                    - Headers per library and platform"
Write-Host "  {library}\$Platform\lib\{config}\...         - Libraries per library, platform, and configuration"
Write-Host ""

# Create target directory if it doesn't exist
if (-not $DryRun) {
    if (-not (Test-Path $Target)) {
        New-Item -ItemType Directory -Path $Target -Force | Out-Null
    }
    $Target = (Resolve-Path $Target).Path
}

# Build list of config directories to process
$configDirs = @()
foreach ($arch in $archList) {
    foreach ($c in $configs) {
        foreach ($l in $linkTypes) {
            if ($Platform -eq "win") {
                $configDirName = "${arch}_${vsToolset}_${l}_${c}"
                $targetConfigName = "${Platform}_${arch}_${vsToolset}_${l}_${c}"
            } else {
                $configDirName = "${arch}_${l}_${c}"
                $targetConfigName = "${Platform}_${arch}_${l}_${c}"
            }
            $configDir = Join-Path $sourcePlatformDir $configDirName

            if (Test-Path $configDir) {
                $configDirs += @{
                    SourceDir = $configDir
                    ConfigName = $configDirName
                    TargetConfigName = $targetConfigName
                    BuildType = $c
                    Arch = $arch
                }
            } else {
                Write-Host "Warning: Source directory not found: $configDir" -ForegroundColor Yellow
            }
        }
    }
}

if ($configDirs.Count -eq 0) {
    Write-Host "ERROR: No valid source configurations found." -ForegroundColor Red
    if ($Platform -eq "win") {
        Write-Host "Make sure build_thirdparty_windows.ps1 was run with -Subdivide ON" -ForegroundColor Yellow
    } else {
        Write-Host "Make sure build_thirdparty_android.ps1 was run with -Subdivide ON" -ForegroundColor Yellow
    }
    exit 1
}

Write-Host "Configurations to process:" -ForegroundColor Yellow
foreach ($cfg in $configDirs) {
    Write-Host "  - $($cfg.ConfigName) -> lib\$($cfg.TargetConfigName)"
}
Write-Host ""

# Get list of libraries from the first config directory
$firstConfigDir = $configDirs[0].SourceDir
$libraries = Get-ChildItem -Path $firstConfigDir -Directory | Select-Object -ExpandProperty Name

Write-Host "Libraries to reorganize: $($libraries -join ', ')" -ForegroundColor Yellow
Write-Host ""

# Track statistics
$stats = @{
    HeadersCopied = 0
    LibsCopied = 0
    DebugSymbolsCopied = 0
    Errors = 0
}

# Track which libraries have had headers copied (only need to copy once per platform)
$headersCopied = @{}

# Determine library file extensions based on platform
if ($Platform -eq "win") {
    $libExtensions = @("*.lib")
    $debugSymbolExtensions = @("*.pdb")
} else {
    $libExtensions = @("*.a", "*.so")
    $debugSymbolExtensions = @()
}

# Process each library
foreach ($lib in $libraries) {
    Write-Host "Processing: $lib" -ForegroundColor Cyan

    # Library-first target directories:
    # {Target}/{library}/{platform}/h/...
    # {Target}/{library}/{platform}/lib/{config}/...
    $libTargetDir = Join-Path $Target $lib
    $libPlatformDir = Join-Path $libTargetDir $Platform
    $libHeadersTarget = Join-Path $libPlatformDir "h"
    $libLibsTarget = Join-Path $libPlatformDir "lib"

    # Clean library platform directory if requested
    if ($Clean -and (Test-Path $libPlatformDir)) {
        if ($DryRun) {
            Write-Host "  [DRY RUN] Would remove: $libPlatformDir" -ForegroundColor Yellow
        } else {
            Remove-Item -Path $libPlatformDir -Recurse -Force
        }
    }

    # Process each configuration
    foreach ($cfg in $configDirs) {
        $libSourceDir = Join-Path $cfg.SourceDir $lib

        if (-not (Test-Path $libSourceDir)) {
            Write-Host "  Warning: Library not found in $($cfg.ConfigName): $lib" -ForegroundColor Yellow
            continue
        }

        # Copy headers (only once per library-platform, from the first config that has them)
        if (-not $headersCopied[$lib]) {
            $libHeadersSource = Join-Path $libSourceDir "include"
            if (Test-Path $libHeadersSource) {
                if ($DryRun) {
                    Write-Host "  [DRY RUN] Would copy headers to: $libHeadersTarget" -ForegroundColor Gray
                    $headerCount = (Get-ChildItem -Path $libHeadersSource -Recurse -File).Count
                    Write-Host "            ($headerCount header files)" -ForegroundColor Gray
                } else {
                    if (-not (Test-Path $libHeadersTarget)) {
                        New-Item -ItemType Directory -Path $libHeadersTarget -Force | Out-Null
                    }
                    # Copy all contents from include/ to h/ preserving structure
                    $headerFiles = Get-ChildItem -Path $libHeadersSource -Recurse -File
                    foreach ($headerFile in $headerFiles) {
                        $relativePath = $headerFile.FullName.Substring($libHeadersSource.Length + 1)
                        $targetPath = Join-Path $libHeadersTarget $relativePath
                        $targetDir = Split-Path -Parent $targetPath
                        if (-not (Test-Path $targetDir)) {
                            New-Item -ItemType Directory -Path $targetDir -Force | Out-Null
                        }
                        Copy-Item -Path $headerFile.FullName -Destination $targetPath -Force
                        $stats.HeadersCopied++
                    }
                    Write-Host "  Copied headers ($($headerFiles.Count) files)" -ForegroundColor Green
                }
                $headersCopied[$lib] = $true
            }
        }

        # Copy libraries to config-specific directory: {library}/{platform}/lib/{config}/
        $libLibSource = Join-Path $libSourceDir "lib"
        if (Test-Path $libLibSource) {
            $configLibTarget = Join-Path $libLibsTarget $cfg.TargetConfigName

            if ($DryRun) {
                $libFileCount = 0
                $debugSymbolCount = 0
                foreach ($ext in $libExtensions) {
                    $libFileCount += (Get-ChildItem -Path $libLibSource -Filter $ext -File -ErrorAction SilentlyContinue).Count
                }
                foreach ($ext in $debugSymbolExtensions) {
                    $debugSymbolCount += (Get-ChildItem -Path $libLibSource -Filter $ext -File -ErrorAction SilentlyContinue).Count
                }
                Write-Host "  [DRY RUN] Would copy libs to: $configLibTarget" -ForegroundColor Gray
                Write-Host "            ($libFileCount lib files, $debugSymbolCount debug symbol files)" -ForegroundColor Gray
            } else {
                if (-not (Test-Path $configLibTarget)) {
                    New-Item -ItemType Directory -Path $configLibTarget -Force | Out-Null
                }

                # Copy library files
                foreach ($ext in $libExtensions) {
                    $libFiles = Get-ChildItem -Path $libLibSource -Filter $ext -File -ErrorAction SilentlyContinue
                    foreach ($libFile in $libFiles) {
                        Copy-Item -Path $libFile.FullName -Destination $configLibTarget -Force
                        $stats.LibsCopied++
                    }
                }

                # Copy debug symbol files
                foreach ($ext in $debugSymbolExtensions) {
                    $debugFiles = Get-ChildItem -Path $libLibSource -Filter $ext -File -ErrorAction SilentlyContinue
                    foreach ($debugFile in $debugFiles) {
                        Copy-Item -Path $debugFile.FullName -Destination $configLibTarget -Force
                        $stats.DebugSymbolsCopied++
                    }
                }

                # Also check for libs in subdirectories (some libraries like wxWidgets have nested lib folders)
                $subDirs = Get-ChildItem -Path $libLibSource -Directory -ErrorAction SilentlyContinue
                foreach ($subDir in $subDirs) {
                    # Skip cmake and pkgconfig directories
                    if ($subDir.Name -in @("cmake", "pkgconfig")) {
                        continue
                    }

                    foreach ($ext in $libExtensions) {
                        $subLibFiles = Get-ChildItem -Path $subDir.FullName -Filter $ext -Recurse -File -ErrorAction SilentlyContinue
                        foreach ($libFile in $subLibFiles) {
                            Copy-Item -Path $libFile.FullName -Destination $configLibTarget -Force
                            $stats.LibsCopied++
                        }
                    }

                    foreach ($ext in $debugSymbolExtensions) {
                        $subDebugFiles = Get-ChildItem -Path $subDir.FullName -Filter $ext -Recurse -File -ErrorAction SilentlyContinue
                        foreach ($debugFile in $subDebugFiles) {
                            Copy-Item -Path $debugFile.FullName -Destination $configLibTarget -Force
                            $stats.DebugSymbolsCopied++
                        }
                    }
                }

                $totalLibs = 0
                foreach ($ext in $libExtensions) {
                    $totalLibs += (Get-ChildItem -Path $libLibSource -Filter $ext -File -ErrorAction SilentlyContinue).Count
                    $totalLibs += ($subDirs | ForEach-Object { (Get-ChildItem -Path $_.FullName -Filter $ext -Recurse -File -ErrorAction SilentlyContinue).Count } | Measure-Object -Sum).Sum
                }
                Write-Host "  Copied libs for $($cfg.TargetConfigName) ($totalLibs files)" -ForegroundColor Green
            }
        }
    }
}

# Summary
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Reorganization Summary" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

if ($DryRun) {
    Write-Host "DRY RUN completed - no files were copied." -ForegroundColor Yellow
} else {
    Write-Host "Headers copied:       $($stats.HeadersCopied)" -ForegroundColor Green
    Write-Host "Libraries copied:     $($stats.LibsCopied)" -ForegroundColor Green
    if ($Platform -eq "win") {
        Write-Host "PDB files copied:     $($stats.DebugSymbolsCopied)" -ForegroundColor Green
    }

    if ($stats.Errors -gt 0) {
        Write-Host "Errors: $($stats.Errors)" -ForegroundColor Red
    }

    Write-Host ""
    Write-Host "Target directory: $Target" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Structure (library-first):" -ForegroundColor Yellow
    Write-Host "  {library}\$Platform\h\...                     - Headers per library and platform"
    if ($Platform -eq "win") {
        Write-Host "  {library}\$Platform\lib\${Platform}_{arch}_{vsToolset}_{link}_{config}\  - Libraries"
    } else {
        Write-Host "  {library}\$Platform\lib\${Platform}_{arch}_{link}_{config}\  - Libraries"
    }
}

exit 0
