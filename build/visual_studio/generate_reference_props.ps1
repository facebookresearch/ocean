# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

<#
.SYNOPSIS
    Generates *_reference.props files from *_common.props and application *_build.props files for Ocean Visual Studio projects.

.DESCRIPTION
    This script parses props files to extract dependency information from *_use.props imports,
    then generates corresponding *_reference.props files with ProjectReference elements.

    For libraries:
    - Parses *_common.props files
    - Generates *_reference.props files
    - Optionally updates *_build.props files to import the generated reference files

    For applications:
    - Parses *_build.props files directly (applications don't have *_common.props)
    - Generates *_reference.props files
    - Optionally updates *_build.props files to import the generated reference files

    The script supports multiple Visual Studio project directories:
    - win\vc143: Windows desktop builds
    - android\vc143: Android builds

.PARAMETER RootPath
    Optional. If specified, processes only the given directory.
    If not specified, processes all known platform directories (win, android).

.PARAMETER UpdateBuildProps
    If specified, also updates *_build.props files to import the generated *_reference.props files.

.EXAMPLE
    .\generate_reference_props.ps1
    Processes all Visual Studio project directories.

.EXAMPLE
    .\generate_reference_props.ps1 -RootPath ".\win\vc143"
    Processes only the Windows vc143 directory.

.EXAMPLE
    .\generate_reference_props.ps1 -UpdateBuildProps
    Processes all directories and updates *_build.props files.

.NOTES
    Author: Ocean Build System
    This script should be run from the build\visual_studio directory.
#>

param(
    [string]$RootPath,
    [switch]$UpdateBuildProps
)

# Helper function to get relative path (compatible with Windows PowerShell and PowerShell Core)
function Get-RelativePath {
    param(
        [string]$FromPath,
        [string]$ToPath
    )

    # Try to use the .NET Core method if available
    try {
        return [System.IO.Path]::GetRelativePath($FromPath, $ToPath)
    } catch {
        # Fallback for Windows PowerShell (.NET Framework)
        # Normalize paths
        $FromPath = [System.IO.Path]::GetFullPath($FromPath).TrimEnd('\', '/')
        $ToPath = [System.IO.Path]::GetFullPath($ToPath)

        # Split paths into parts
        $fromParts = $FromPath -split '[\\\/]'
        $toParts = $ToPath -split '[\\\/]'

        # Find common prefix length
        $commonLength = 0
        $minLength = [Math]::Min($fromParts.Length, $toParts.Length)
        for ($i = 0; $i -lt $minLength; $i++) {
            if ($fromParts[$i] -ieq $toParts[$i]) {
                $commonLength++
            } else {
                break
            }
        }

        # Build relative path
        $upCount = $fromParts.Length - $commonLength
        $relativeParts = @()

        # Add ".." for each level up
        for ($i = 0; $i -lt $upCount; $i++) {
            $relativeParts += ".."
        }

        # Add remaining path parts
        for ($i = $commonLength; $i -lt $toParts.Length; $i++) {
            $relativeParts += $toParts[$i]
        }

        if ($relativeParts.Length -eq 0) {
            return "."
        }

        return $relativeParts -join "\"
    }
}

# Parse a props file to extract dependencies from *_use.props imports
function Get-Dependencies {
    param(
        [string]$PropsPath
    )

    $dependencies = @()

    if (-not (Test-Path $PropsPath)) {
        return $dependencies
    }

    $content = Get-Content $PropsPath -Raw
    $directory = Split-Path $PropsPath -Parent

    # Match Import statements for *_use.props files (but not ocean_common.props)
    $pattern = '<Import\s+Condition="[^"]*"\s+Project="([^"]*_use\.props)"'
    $matches = [regex]::Matches($content, $pattern)

    foreach ($match in $matches) {
        $usePropsRelativePath = $match.Groups[1].Value

        # Resolve the path to the use.props file
        $usePropsFullPath = [System.IO.Path]::GetFullPath([System.IO.Path]::Combine($directory, $usePropsRelativePath))

        # The vcxproj is in the same directory as the _use.props, with matching base name
        $usePropsDir = Split-Path $usePropsFullPath -Parent
        $usePropsName = [System.IO.Path]::GetFileNameWithoutExtension($usePropsFullPath)

        # Remove _use suffix to get base name
        $baseName = $usePropsName -replace '_use$', ''

        # Find the corresponding vcxproj
        $vcxprojPath = Join-Path $usePropsDir "$baseName.vcxproj"

        if (Test-Path $vcxprojPath) {
            $dependencies += @{
                VcxprojPath = $vcxprojPath
                UsePropsPath = $usePropsFullPath
                RelativeUsePropsPath = $usePropsRelativePath
            }
        }
    }

    return $dependencies
}

# Get the project GUID from a vcxproj file
function Get-ProjectGuid {
    param([string]$VcxprojPath)

    if (-not (Test-Path $VcxprojPath)) {
        return $null
    }

    $content = Get-Content $VcxprojPath -Raw

    if ($content -match '<ProjectGuid>\{([A-F0-9\-]+)\}</ProjectGuid>') {
        return $Matches[1]
    }

    return $null
}

# Generate a *_reference.props file
function Generate-ReferenceProps {
    param(
        [string]$SourcePropsPath,
        [string]$OutputPath,
        [string]$BaseName,
        [string]$DisplayNamePrefix = "Ocean"
    )

    $directory = Split-Path $SourcePropsPath -Parent

    # Get dependencies
    $dependencies = Get-Dependencies -PropsPath $SourcePropsPath

    if ($dependencies.Count -eq 0) {
        Write-Host "  No dependencies found for $BaseName, skipping..."
        return $false
    }

    # Build display name
    $displayName = "$DisplayNamePrefix $($BaseName.Substring(0,1).ToUpper() + $BaseName.Substring(1)) Project References"

    # Build imported property name
    $importedPropertyName = "ocean_$($BaseName)_reference_imported"

    # Start building XML content
    $xml = @"
<?xml version="1.0" encoding="utf-8"?>
<!--
  This file was auto-generated by generate_reference_props.ps1
  Do not edit manually. Re-run the script to regenerate.
-->
<Project ToolsVersion="Current" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup>
    <_PropertySheetDisplayName>$displayName</_PropertySheetDisplayName>
  </PropertyGroup>

  <PropertyGroup>
    <$importedPropertyName>True</$importedPropertyName>
  </PropertyGroup>

  <ItemGroup>
"@

    foreach ($dep in $dependencies) {
        $guid = Get-ProjectGuid -VcxprojPath $dep.VcxprojPath

        if ($null -eq $guid) {
            Write-Warning "  Could not find GUID for $($dep.VcxprojPath)"
            continue
        }

        # Calculate relative path from the output directory to the vcxproj
        $outputDir = Split-Path $OutputPath -Parent
        $relativePath = Get-RelativePath -FromPath $outputDir -ToPath $dep.VcxprojPath
        # Convert to Windows path separators
        $relativePath = $relativePath -replace '/', '\'

        # Use $(MSBuildThisFileDirectory) to make the path relative to this props file's location,
        # not the importing vcxproj's location. This is critical because MSBuild evaluates
        # ProjectReference paths from the vcxproj that imports the props file, not from
        # the props file's directory.
        $xml += @"

    <ProjectReference Include="`$(MSBuildThisFileDirectory)$relativePath">
      <Project>{$guid}</Project>
      <LinkLibraryDependencies>false</LinkLibraryDependencies>
    </ProjectReference>
"@
    }

    $xml += @"

  </ItemGroup>
</Project>
"@

    # Write the file
    $xml | Out-File -FilePath $OutputPath -Encoding utf8
    Write-Host "  Generated: $OutputPath"

    return $true
}

# Update a library *_build.props file to import the *_reference.props (insert after *_common.props)
function Update-LibraryBuildProps {
    param(
        [string]$BuildPropsPath,
        [string]$ReferencePropsPath,
        [string]$BaseName
    )

    if (-not (Test-Path $BuildPropsPath)) {
        Write-Warning "  Build props not found: $BuildPropsPath"
        return $false
    }

    $content = Get-Content $BuildPropsPath -Raw
    $referencePropsName = "$($BaseName)_reference.props"
    $commonPropsName = "$($BaseName)_common.props"
    $importedPropertyName = "ocean_$($BaseName)_reference_imported"

    # Check if already imported
    if ($content -match [regex]::Escape($referencePropsName)) {
        Write-Host "  Already imported in: $BuildPropsPath"
        return $false
    }

    # Build the new import line
    $newImportLine = "    <Import Condition=""`'`$($importedPropertyName)' != 'True'"" Project=""$referencePropsName""/>"

    # Try to find the *_common.props import line and insert after it
    $commonPropsPattern = "(<Import\s+Condition=""[^""]*""\s+Project=""[^""]*$([regex]::Escape($commonPropsName))""\s*/>)"

    if ($content -match $commonPropsPattern) {
        # Insert the reference import directly after the common import
        $newContent = $content -replace $commonPropsPattern, "`$1`n$newImportLine"
        $newContent | Out-File -FilePath $BuildPropsPath -Encoding utf8 -NoNewline
        Write-Host "  Updated: $BuildPropsPath"
        return $true
    } else {
        # Fallback: insert at the end of ImportGroup if common.props not found
        $pattern = '(<ImportGroup\s+Label="Dependencies">[\s\S]*?)(</ImportGroup>)'

        if ($content -match $pattern) {
            $newContent = $content -replace $pattern, "`$1$newImportLine`n  `$2"
            $newContent | Out-File -FilePath $BuildPropsPath -Encoding utf8 -NoNewline
            Write-Host "  Updated: $BuildPropsPath (fallback - appended to ImportGroup)"
            return $true
        } else {
            Write-Warning "  Could not find ImportGroup in: $BuildPropsPath"
            return $false
        }
    }
}

# Update an application *_build.props file to import the *_reference.props (insert at start of ImportGroup)
function Update-ApplicationBuildProps {
    param(
        [string]$BuildPropsPath,
        [string]$ReferencePropsPath,
        [string]$BaseName
    )

    if (-not (Test-Path $BuildPropsPath)) {
        Write-Warning "  Build props not found: $BuildPropsPath"
        return $false
    }

    $content = Get-Content $BuildPropsPath -Raw
    $referencePropsName = "$($BaseName)_reference.props"
    $importedPropertyName = "ocean_$($BaseName)_reference_imported"

    # Check if already imported
    if ($content -match [regex]::Escape($referencePropsName)) {
        Write-Host "  Already imported in: $BuildPropsPath"
        return $false
    }

    # Build the new import line
    $newImportLine = "    <Import Condition=""`'`$($importedPropertyName)' != 'True'"" Project=""$referencePropsName""/>"

    # Insert at the beginning of the ImportGroup (after the opening tag)
    $pattern = '(<ImportGroup\s+Label="Dependencies">)\r?\n'

    if ($content -match $pattern) {
        $newContent = $content -replace $pattern, "`$1`n$newImportLine`n"
        $newContent | Out-File -FilePath $BuildPropsPath -Encoding utf8 -NoNewline
        Write-Host "  Updated: $BuildPropsPath"
        return $true
    } else {
        Write-Warning "  Could not find ImportGroup in: $BuildPropsPath"
        return $false
    }
}

# Check if a build.props file is for an application (no corresponding common.props)
function Test-IsApplicationBuildProps {
    param(
        [string]$BuildPropsPath
    )

    $directory = Split-Path $BuildPropsPath -Parent
    $baseName = [System.IO.Path]::GetFileNameWithoutExtension($BuildPropsPath) -replace '_build$', ''
    $commonPropsPath = Join-Path $directory "$($baseName)_common.props"

    return -not (Test-Path $commonPropsPath)
}

# Check if a build.props file has its own *_use.props imports (not just importing other build.props)
function Test-HasDirectDependencies {
    param(
        [string]$BuildPropsPath
    )

    if (-not (Test-Path $BuildPropsPath)) {
        return $false
    }

    $content = Get-Content $BuildPropsPath -Raw

    # Check if it has *_use.props imports
    return $content -match '<Import\s+Condition="[^"]*"\s+Project="[^"]*_use\.props"'
}

# Process a single directory
function Process-Directory {
    param(
        [string]$DirectoryPath,
        [switch]$UpdateBuildProps
    )

    Write-Host ""
    Write-Host "Processing directory: $DirectoryPath"
    Write-Host "------------------------------------------------"

    if (-not (Test-Path $DirectoryPath)) {
        Write-Warning "Directory not found: $DirectoryPath"
        return @{ Generated = 0; Updated = 0 }
    }

    $generatedCount = 0
    $updatedCount = 0

    # Part 1: Process library *_common.props files
    Write-Host ""
    Write-Host "Processing library *_common.props files..."
    Write-Host ""

    $commonPropsFiles = Get-ChildItem -Path $DirectoryPath -Filter "*_common.props" -Recurse | Where-Object {
        # Exclude ocean_common.props (it's the root common, not a library)
        $_.Name -ne "ocean_common.props"
    }

    Write-Host "Found $($commonPropsFiles.Count) *_common.props files"

    foreach ($commonProps in $commonPropsFiles) {
        $directory = $commonProps.Directory.FullName
        $baseName = $commonProps.BaseName -replace '_common$', ''

        Write-Host "Processing: $($commonProps.Name)"

        # Generate reference props
        $referencePropsPath = Join-Path $directory "$($baseName)_reference.props"

        $generated = Generate-ReferenceProps -SourcePropsPath $commonProps.FullName -OutputPath $referencePropsPath -BaseName $baseName

        if ($generated) {
            $generatedCount++

            # Optionally update build props
            if ($UpdateBuildProps) {
                $buildPropsPath = Join-Path $directory "$($baseName)_build.props"
                $updated = Update-LibraryBuildProps -BuildPropsPath $buildPropsPath -ReferencePropsPath $referencePropsPath -BaseName $baseName
                if ($updated) {
                    $updatedCount++
                }
            }
        }
    }

    # Part 2: Process application *_build.props files (those without corresponding *_common.props)
    Write-Host ""
    Write-Host "Processing application *_build.props files..."
    Write-Host ""

    $buildPropsFiles = Get-ChildItem -Path $DirectoryPath -Filter "*_build.props" -Recurse | Where-Object {
        # Only process build props that are for applications (no common.props)
        # AND have direct *_use.props dependencies
        (Test-IsApplicationBuildProps -BuildPropsPath $_.FullName) -and
        (Test-HasDirectDependencies -BuildPropsPath $_.FullName)
    }

    Write-Host "Found $($buildPropsFiles.Count) application *_build.props files with dependencies"

    foreach ($buildProps in $buildPropsFiles) {
        $directory = $buildProps.Directory.FullName
        $baseName = $buildProps.BaseName -replace '_build$', ''

        Write-Host "Processing: $($buildProps.Name)"

        # Generate reference props
        $referencePropsPath = Join-Path $directory "$($baseName)_reference.props"

        $generated = Generate-ReferenceProps -SourcePropsPath $buildProps.FullName -OutputPath $referencePropsPath -BaseName $baseName -DisplayNamePrefix "Application"

        if ($generated) {
            $generatedCount++

            # Optionally update build props
            if ($UpdateBuildProps) {
                $updated = Update-ApplicationBuildProps -BuildPropsPath $buildProps.FullName -ReferencePropsPath $referencePropsPath -BaseName $baseName
                if ($updated) {
                    $updatedCount++
                }
            }
        }
    }

    return @{ Generated = $generatedCount; Updated = $updatedCount }
}

# Main execution
Write-Host "Ocean Visual Studio Project Reference Generator"
Write-Host "================================================"
Write-Host ""

$totalGenerated = 0
$totalUpdated = 0

if ([string]::IsNullOrEmpty($RootPath)) {
    # No RootPath specified - process all known directories
    $visualStudioRoot = $PSScriptRoot

    $directories = @(
        (Join-Path $visualStudioRoot "win\vc143"),
        (Join-Path $visualStudioRoot "android\vc143")
    )

    Write-Host "Processing all platform directories..."

    foreach ($dir in $directories) {
        if (Test-Path $dir) {
            $result = Process-Directory -DirectoryPath $dir -UpdateBuildProps:$UpdateBuildProps
            $totalGenerated += $result.Generated
            $totalUpdated += $result.Updated
        } else {
            Write-Host "Skipping (not found): $dir"
        }
    }
} else {
    # Process only the specified root path
    Write-Host "Root Path: $RootPath"
    $result = Process-Directory -DirectoryPath $RootPath -UpdateBuildProps:$UpdateBuildProps
    $totalGenerated = $result.Generated
    $totalUpdated = $result.Updated
}

Write-Host ""
Write-Host "================================================"
Write-Host "Summary:"
Write-Host "  Generated $totalGenerated *_reference.props files"
if ($UpdateBuildProps) {
    Write-Host "  Updated $totalUpdated *_build.props files"
}
Write-Host ""
Write-Host "Done!"
