# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

#Requires -RunAsAdministrator
<#
.SYNOPSIS
    Installs the OceanNDK Application Type into Visual Studio.

.DESCRIPTION
    This script copies the OceanNDK Application Type files to the Visual Studio MSBuild directories.
    This enables projects to use ApplicationType=OceanNDK with proper property pages.

    The Application Type approach is similar to VS's built-in Android Application Type,
    which properly supports custom property pages in the 1033 locale folder.

    This script requires administrator privileges because it writes to:
    C:\Program Files\Microsoft Visual Studio\<Year>\<Edition>\MSBuild\Microsoft\VC\<Version>\Application Type\

    By default, the script detects and installs to ALL found Visual Studio installations.

.PARAMETER Uninstall
    Remove the OceanNDK Application Type instead of installing it.

.PARAMETER VSVersion
    Specific Visual Studio version year to target. If not specified, all found versions are processed.
    Examples: 2019, 2022, 2026

.PARAMETER VSEdition
    Specific Visual Studio edition to target. If not specified, all found editions are processed.
    Options: Community, Professional, Enterprise

.EXAMPLE
    # Install to all found VS versions (run as Administrator)
    .\InstallOceanNDKToolset.ps1

.EXAMPLE
    # Install only to VS 2022 Professional
    .\InstallOceanNDKToolset.ps1 -VSVersion 2022 -VSEdition Professional

.EXAMPLE
    # Uninstall from all VS versions
    .\InstallOceanNDKToolset.ps1 -Uninstall

.NOTES
    After running this script:
    1. Create a new project or modify an existing .vcxproj file
    2. Set ApplicationType=OceanNDK and ApplicationTypeRevision=1.0
    3. The project will use the OceanNDK toolset with proper property pages
#>

param(
    [switch]$Uninstall,
    [string]$VSVersion,
    [ValidateSet("Community", "Professional", "Enterprise", "")]
    [string]$VSEdition
)

$ErrorActionPreference = "Stop"

# Application Type name and version
$AppTypeName = "OceanNDK"
$AppTypeVersion = "1.0"

# Mapping of VS year to MSBuild VC folder version
$MSBuildVCFolderVersions = @{
    "2019" = "v160"
    "2022" = "v170"
    "2026" = "v180"
}

# Source path for the Application Type files (relative to this script)
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$SourceAppTypePath = Join-Path (Split-Path -Parent $ScriptDir) "apptype\$AppTypeName\$AppTypeVersion"

function Test-AdminPrivileges {
    $identity = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($identity)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

function Find-VSInstallations {
    param(
        [string]$FilterVersion,
        [string]$FilterEdition
    )

    $installations = @()
    $editions = @("Enterprise", "Professional", "Community")
    $years = $MSBuildVCFolderVersions.Keys | Sort-Object -Descending

    if ($FilterVersion) {
        $years = @($FilterVersion)
    }
    if ($FilterEdition) {
        $editions = @($FilterEdition)
    }

    foreach ($year in $years) {
        foreach ($edition in $editions) {
            $vsPath = "C:\Program Files\Microsoft Visual Studio\$year\$edition"

            if (-not (Test-Path $vsPath)) {
                continue
            }

            $vcPath = Join-Path $vsPath "MSBuild\Microsoft\VC"
            $vcVersion = $null

            # First, try the assumed version from our mapping
            $assumedVersion = $MSBuildVCFolderVersions[$year]
            if ($assumedVersion) {
                $assumedPath = Join-Path $vcPath $assumedVersion
                if (Test-Path $assumedPath) {
                    $vcVersion = $assumedVersion
                }
            }

            # Fallback: auto-detect VC version from actual folder
            if (-not $vcVersion -and (Test-Path $vcPath)) {
                $vcFolders = Get-ChildItem -Path $vcPath -Directory -ErrorAction SilentlyContinue | Where-Object { $_.Name -match '^v\d+$' }
                if ($vcFolders) {
                    $vcVersion = ($vcFolders | Sort-Object Name -Descending | Select-Object -First 1).Name
                }
            }

            if (-not $vcVersion) {
                continue
            }

            $vcTargetsPath = Join-Path $vsPath "MSBuild\Microsoft\VC\$vcVersion"

            if (Test-Path $vcTargetsPath) {
                $installations += @{
                    Year = $year
                    Edition = $edition
                    Path = $vsPath
                    VCVersion = $vcVersion
                    VCTargetsPath = $vcTargetsPath
                }
            }
        }
    }

    return $installations
}

function Install-ApplicationType {
    param(
        [hashtable]$VSInstall
    )

    $year = $VSInstall.Year
    $edition = $VSInstall.Edition
    $vcTargetsPath = $VSInstall.VCTargetsPath
    $vcVersion = $VSInstall.VCVersion

    Write-Host "  VS $year $edition ($vcVersion)" -ForegroundColor Cyan

    # Target path: $(VCTargetsPath)\Application Type\OceanNDK\1.0\
    $appTypePath = Join-Path $vcTargetsPath "Application Type\$AppTypeName\$AppTypeVersion"

    # Check source exists
    if (-not (Test-Path $SourceAppTypePath)) {
        Write-Host "    ERROR: Source Application Type not found at: $SourceAppTypePath" -ForegroundColor Red
        return 0
    }

    # Create Application Type directory
    if (-not (Test-Path $appTypePath)) {
        New-Item -ItemType Directory -Path $appTypePath -Force | Out-Null
    }

    # Copy all files recursively
    try {
        Copy-Item -Path "$SourceAppTypePath\*" -Destination $appTypePath -Recurse -Force
        Write-Host "    Installed Application Type to: $appTypePath" -ForegroundColor Green
        return 1
    }
    catch {
        Write-Host "    ERROR: Failed to copy files: $_" -ForegroundColor Red
        return 0
    }
}

function Uninstall-ApplicationType {
    param(
        [hashtable]$VSInstall
    )

    $year = $VSInstall.Year
    $edition = $VSInstall.Edition
    $vcTargetsPath = $VSInstall.VCTargetsPath
    $vcVersion = $VSInstall.VCVersion

    Write-Host "  VS $year $edition ($vcVersion)" -ForegroundColor Cyan

    $appTypePath = Join-Path $vcTargetsPath "Application Type\$AppTypeName"

    if (Test-Path $appTypePath) {
        try {
            Remove-Item -Path $appTypePath -Recurse -Force
            Write-Host "    Removed Application Type from: $appTypePath" -ForegroundColor Green
            return 1
        }
        catch {
            Write-Host "    ERROR: Failed to remove: $_" -ForegroundColor Red
            return 0
        }
    } else {
        Write-Host "    Not installed" -ForegroundColor Gray
        return 0
    }
}

# Registry key where the extension stores its path
$ExtensionRegistryKey = "HKCU:\Software\OceanAndroidExtension"

function Test-ExtensionRegistered {
    <#
    .SYNOPSIS
        Checks if the Ocean Android Extension has registered itself in the registry.
    .DESCRIPTION
        The extension writes its install path to the registry when Visual Studio starts.
        If this key doesn't exist, the user hasn't started VS with the extension installed yet.
    #>
    if (-not (Test-Path $ExtensionRegistryKey)) {
        return $false
    }

    $extensionPath = Get-ItemProperty -Path $ExtensionRegistryKey -Name "ExtensionPath" -ErrorAction SilentlyContinue
    return ($null -ne $extensionPath)
}

# Main execution
Write-Host ""
Write-Host "=============================================" -ForegroundColor Cyan
Write-Host "  OceanNDK Application Type Installer" -ForegroundColor Cyan
Write-Host "=============================================" -ForegroundColor Cyan
Write-Host ""

# Check admin privileges
if (-not (Test-AdminPrivileges)) {
    Write-Error "This script requires administrator privileges.`nPlease run PowerShell as Administrator and try again."
    exit 1
}

# Check if extension has been registered (VS started with extension installed)
if (-not $Uninstall) {
    Write-Host "Checking extension registration..." -ForegroundColor Gray
    if (-not (Test-ExtensionRegistered)) {
        Write-Host ""
        Write-Host "WARNING: Ocean Android Extension registry key not found!" -ForegroundColor Yellow
        Write-Host ""
        Write-Host "This usually means one of the following:" -ForegroundColor White
        Write-Host "  1. The VSIX extension has not been installed yet" -ForegroundColor White
        Write-Host "  2. Visual Studio has not been started after installing the extension" -ForegroundColor White
        Write-Host ""
        Write-Host "Recommended steps:" -ForegroundColor Cyan
        Write-Host "  1. Install the OceanAndroidExtension.vsix (double-click or use VS installer)" -ForegroundColor White
        Write-Host "  2. Start Visual Studio at least once (extension registers on startup)" -ForegroundColor White
        Write-Host "  3. Close Visual Studio" -ForegroundColor White
        Write-Host "  4. Run this script again" -ForegroundColor White
        Write-Host ""
        Write-Host "Expected registry key: $ExtensionRegistryKey" -ForegroundColor Gray
        Write-Host ""

        $response = Read-Host "Continue anyway? (y/N)"
        if ($response -ne 'y' -and $response -ne 'Y') {
            Write-Host "Installation cancelled." -ForegroundColor Yellow
            exit 0
        }
        Write-Host ""
    } else {
        $regProps = Get-ItemProperty -Path $ExtensionRegistryKey
        Write-Host "Extension registered at: $($regProps.ExtensionPath)" -ForegroundColor Green
    }
}

# Verify source path exists
Write-Host "Source Application Type: $SourceAppTypePath" -ForegroundColor Gray
if (-not (Test-Path $SourceAppTypePath)) {
    Write-Error "Source Application Type not found at: $SourceAppTypePath`nEnsure you're running this script from the correct location."
    exit 1
}

# Find all VS installations
Write-Host "Searching for Visual Studio installations..." -ForegroundColor Gray
$vsInstallations = Find-VSInstallations -FilterVersion $VSVersion -FilterEdition $VSEdition

if ($vsInstallations.Count -eq 0) {
    if ($VSVersion -or $VSEdition) {
        Write-Error "No Visual Studio installation found matching the specified criteria.`nVersion: $(if ($VSVersion) { $VSVersion } else { 'any' }), Edition: $(if ($VSEdition) { $VSEdition } else { 'any' })"
    } else {
        Write-Error "No Visual Studio installations found with C++ workload installed."
    }
    exit 1
}

Write-Host "Found $($vsInstallations.Count) Visual Studio installation(s):" -ForegroundColor Green
foreach ($vs in $vsInstallations) {
    Write-Host "  - VS $($vs.Year) $($vs.Edition) ($($vs.VCVersion))" -ForegroundColor Gray
}
Write-Host ""

$totalCount = 0

if ($Uninstall) {
    Write-Host "Uninstalling $AppTypeName Application Type..." -ForegroundColor Cyan
    Write-Host ""

    foreach ($vs in $vsInstallations) {
        $totalCount += Uninstall-ApplicationType -VSInstall $vs
    }

    Write-Host ""
    if ($totalCount -gt 0) {
        Write-Host "Successfully removed from $totalCount installation(s)." -ForegroundColor Green
    } else {
        Write-Host "No $AppTypeName Application Type found to remove." -ForegroundColor Yellow
    }
} else {
    Write-Host "Installing $AppTypeName Application Type..." -ForegroundColor Cyan
    Write-Host ""

    foreach ($vs in $vsInstallations) {
        $totalCount += Install-ApplicationType -VSInstall $vs
    }

    Write-Host ""
    if ($totalCount -gt 0) {
        Write-Host "Successfully installed to $totalCount installation(s)." -ForegroundColor Green
        Write-Host ""
        Write-Host "To use the OceanNDK Application Type in a project:" -ForegroundColor Yellow
        Write-Host "  1. Open/create a .vcxproj file" -ForegroundColor White
        Write-Host "  2. Add the following properties in the first PropertyGroup:" -ForegroundColor White
        Write-Host ""
        Write-Host "     <ApplicationType>$AppTypeName</ApplicationType>" -ForegroundColor Cyan
        Write-Host "     <ApplicationTypeRevision>$AppTypeVersion</ApplicationTypeRevision>" -ForegroundColor Cyan
        Write-Host ""
        Write-Host "  3. Remove any existing <PlatformToolset> element (will default to OceanNDK)" -ForegroundColor White
        Write-Host "  4. Set platform to ARM64, ARM, x64, or x86" -ForegroundColor White
        Write-Host ""
        Write-Host "Example .vcxproj snippet:" -ForegroundColor Yellow
        Write-Host @"
  <PropertyGroup Label="Globals">
    <VCProjectVersion>16.0</VCProjectVersion>
    <ProjectGuid>{GUID-HERE}</ProjectGuid>
    <RootNamespace>MyAndroidLib</RootNamespace>
    <ApplicationType>$AppTypeName</ApplicationType>
    <ApplicationTypeRevision>$AppTypeVersion</ApplicationTypeRevision>
  </PropertyGroup>
"@ -ForegroundColor Gray
    } else {
        Write-Error "No installations were successful. Check error messages above."
        exit 1
    }
}

Write-Host ""
exit 0
