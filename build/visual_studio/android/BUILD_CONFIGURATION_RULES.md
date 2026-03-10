# Visual Studio Solution Build Configuration Rules

This document describes the build configuration rules for the Android Visual Studio solution (`android_vc143.sln`).

## Project Types

The solution contains four types of projects:

| Type | Description | Examples |
|------|-------------|----------|
| **Native Library** | Core C++ libraries that compile to static or shared libraries | `base`, `math`, `cv`, `geometry`, `tracking`, `media`, `rendering` |
| **Java Library** | Android Java libraries | (if any exist) |
| **App Native Library** | The specific native library that packages/wraps functionality for a particular Java app | `consolenative` (for `consoleapp`) |
| **Java App** | Android Java applications (APK packaging projects) | `consoleapp` |

## Solution Configurations

The solution defines 6 configurations across 3 platforms:

**Configurations:**
- `Static Debug` / `Static Release` - For building standalone native libraries
- `Shared Debug` / `Shared Release` - For building native libraries + app native libraries
- `Debug` / `Release` - For building the complete app (Java + native)

**Platforms:**
- `ARM` - 32-bit ARM (armeabi-v7a)
- `ARM64` - 64-bit ARM (arm64-v8a)
- `x64` - x86_64

## Build Configuration Matrix

When a solution configuration is selected, each project type should behave as follows:

| Solution Config | Native Library | App Native Library | Java Library | Java App |
|-----------------|----------------|-------------------|--------------|----------|
| **Static Debug** | `Static Debug` ✅ BUILD | ❌ NO BUILD | ❌ NO BUILD | ❌ NO BUILD |
| **Static Release** | `Static Release` ✅ BUILD | ❌ NO BUILD | ❌ NO BUILD | ❌ NO BUILD |
| **Shared Debug** | `Static Debug` ✅ BUILD | `Shared Debug` ✅ BUILD | ❌ NO BUILD | ❌ NO BUILD |
| **Shared Release** | `Static Release` ✅ BUILD | `Shared Release` ✅ BUILD | ❌ NO BUILD | ❌ NO BUILD |
| **Debug** | `Static Debug` ✅ BUILD | `Shared Debug` ✅ BUILD | `Debug` ✅ BUILD | `Debug` ✅ BUILD |
| **Release** | `Static Release` ✅ BUILD | `Shared Release` ✅ BUILD | `Release` ✅ BUILD | `Release` ✅ BUILD |

### Key Rules

1. **Native Libraries are ALWAYS built as Static**
   - Even when `Shared Debug/Release` or `Debug/Release` is selected
   - This is because shared app libraries link against static native libraries

2. **App Native Libraries are built as Shared**
   - Only when `Shared Debug/Release` or `Debug/Release` is selected
   - They are NOT built for `Static Debug/Release`

3. **Java components only build for Debug/Release**
   - Java libraries and Java apps are not built for Static or Shared configurations

4. **Debug configs must NEVER map to Release configs (and vice versa)**
   - `Static Debug`, `Shared Debug`, `Debug` → must map to Debug variants
   - `Static Release`, `Shared Release`, `Release` → must map to Release variants

## Solution File Format

In the `.sln` file, project configurations are defined in the `ProjectConfigurationPlatforms` section:

```text
GlobalSection(ProjectConfigurationPlatforms) = postSolution
    {GUID}.SolutionConfig|Platform.ActiveCfg = ProjectConfig|Platform
    {GUID}.SolutionConfig|Platform.Build.0 = ProjectConfig|Platform
    {GUID}.SolutionConfig|Platform.Deploy.0 = ProjectConfig|Platform
EndGlobalSection
```

- `ActiveCfg` - Which project configuration is active (always present)
- `Build.0` - If present, project will be BUILT when this solution config is selected
- `Deploy.0` - If present, project will be DEPLOYED (only for Java apps)

## Examples

### Example 1: Native Library (e.g., `base`)

The `base` project has configurations: `Static Debug`, `Static Release`, `Shared Debug`, `Shared Release`

```text
# Static Debug - BUILD with Static Debug
{GUID}.Static Debug|ARM64.ActiveCfg = Static Debug|ARM64
{GUID}.Static Debug|ARM64.Build.0 = Static Debug|ARM64

# Static Release - BUILD with Static Release
{GUID}.Static Release|ARM64.ActiveCfg = Static Release|ARM64
{GUID}.Static Release|ARM64.Build.0 = Static Release|ARM64

# Shared Debug - BUILD with Static Debug (NOT Shared Debug!)
{GUID}.Shared Debug|ARM64.ActiveCfg = Static Debug|ARM64
{GUID}.Shared Debug|ARM64.Build.0 = Static Debug|ARM64

# Shared Release - BUILD with Static Release (NOT Shared Release!)
{GUID}.Shared Release|ARM64.ActiveCfg = Static Release|ARM64
{GUID}.Shared Release|ARM64.Build.0 = Static Release|ARM64

# Debug - BUILD with Static Debug
{GUID}.Debug|ARM64.ActiveCfg = Static Debug|ARM64
{GUID}.Debug|ARM64.Build.0 = Static Debug|ARM64

# Release - BUILD with Static Release
{GUID}.Release|ARM64.ActiveCfg = Static Release|ARM64
{GUID}.Release|ARM64.Build.0 = Static Release|ARM64
```

### Example 2: App Native Library (e.g., `consolenative`)

The `consolenative` project has configurations: `Static Debug`, `Static Release`, `Shared Debug`, `Shared Release`

```text
# Static Debug - NO BUILD (ActiveCfg only, no Build.0)
{GUID}.Static Debug|ARM64.ActiveCfg = Static Debug|ARM64
# (no Build.0 line = not built)

# Static Release - NO BUILD
{GUID}.Static Release|ARM64.ActiveCfg = Static Release|ARM64
# (no Build.0 line = not built)

# Shared Debug - BUILD with Shared Debug
{GUID}.Shared Debug|ARM64.ActiveCfg = Shared Debug|ARM64
{GUID}.Shared Debug|ARM64.Build.0 = Shared Debug|ARM64

# Shared Release - BUILD with Shared Release
{GUID}.Shared Release|ARM64.ActiveCfg = Shared Release|ARM64
{GUID}.Shared Release|ARM64.Build.0 = Shared Release|ARM64

# Debug - BUILD with Shared Debug
{GUID}.Debug|ARM64.ActiveCfg = Shared Debug|ARM64
{GUID}.Debug|ARM64.Build.0 = Shared Debug|ARM64

# Release - BUILD with Shared Release
{GUID}.Release|ARM64.ActiveCfg = Shared Release|ARM64
{GUID}.Release|ARM64.Build.0 = Shared Release|ARM64
```

### Example 3: Java App (e.g., `consoleapp`)

The `consoleapp` project has configurations: `Debug`, `Release` only

```text
# Static Debug - NO BUILD
{GUID}.Static Debug|ARM64.ActiveCfg = Debug|ARM64
# (no Build.0 line = not built)

# Static Release - NO BUILD
{GUID}.Static Release|ARM64.ActiveCfg = Release|ARM64
# (no Build.0 line = not built)

# Shared Debug - NO BUILD
{GUID}.Shared Debug|ARM64.ActiveCfg = Debug|ARM64
# (no Build.0 line = not built)

# Shared Release - NO BUILD
{GUID}.Shared Release|ARM64.ActiveCfg = Release|ARM64
# (no Build.0 line = not built)

# Debug - BUILD with Debug (+ Deploy)
{GUID}.Debug|ARM64.ActiveCfg = Debug|ARM64
{GUID}.Debug|ARM64.Build.0 = Debug|ARM64
{GUID}.Debug|ARM64.Deploy.0 = Debug|ARM64

# Release - BUILD with Release (+ Deploy)
{GUID}.Release|ARM64.ActiveCfg = Release|ARM64
{GUID}.Release|ARM64.Build.0 = Release|ARM64
{GUID}.Release|ARM64.Deploy.0 = Release|ARM64
```

## Configuration Mapping Rules (Closest Match)

When a project doesn't have an exact matching configuration, map to the closest available:

| Project Has | Solution Config | Maps To |
|-------------|-----------------|---------|
| Static/Shared Debug/Release | `Debug` | `Shared Debug` |
| Static/Shared Debug/Release | `Release` | `Shared Release` |
| Debug/Release only | `Static Debug` | `Debug` |
| Debug/Release only | `Static Release` | `Release` |
| Debug/Release only | `Shared Debug` | `Debug` |
| Debug/Release only | `Shared Release` | `Release` |

**CRITICAL: Never cross Debug/Release boundaries!**
- Debug configs (`Debug`, `Static Debug`, `Shared Debug`) → map to Debug variants only
- Release configs (`Release`, `Static Release`, `Shared Release`) → map to Release variants only

## Verification Checklist

When verifying the solution file, check:

1. ☐ Every `Debug|*`, `Static Debug|*`, `Shared Debug|*` maps to a `*Debug*` project config
2. ☐ Every `Release|*`, `Static Release|*`, `Shared Release|*` maps to a `*Release*` project config
3. ☐ Native libraries have `Build.0` for ALL solution configs (they always build)
4. ☐ App native libraries have `Build.0` ONLY for `Shared *` and `Debug`/`Release` configs
5. ☐ Java apps have `Build.0` ONLY for `Debug`/`Release` configs
6. ☐ Java apps have `Deploy.0` for `Debug`/`Release` configs
7. ☐ Native libraries use `Static Debug/Release` for `Shared *` and `Debug/Release` solution configs

## Project Classification

To determine a project's type, check its `.vcxproj` file:

- **Native Library**: Has `<ConfigurationType>StaticLibrary</ConfigurationType>` or `<ConfigurationType>DynamicLibrary</ConfigurationType>` with both Static and Shared configs
- **App Native Library**: Native library that is a dependency of a Java app (check solution dependencies)
- **Java App**: Has `<ConfigurationType>Makefile</ConfigurationType>` with Android packaging, or is an `.androidproj`

## Current Known Projects

### Native Libraries
- `base`, `math`, `cv`, `geometry`, `io`, `media`, `network`, `platform`, `rendering`, `system`, `tracking`, `interaction`, `scenedescription`
- All `cv*` submodules (cvadvanced, cvdetector, etc.)
- All `tracking*` submodules
- All `test*` projects
- `platformandroid`, `platformlinux`
- All JNI libraries: `basejni`, `mediajni`, `networkjni`, `renderingglescenegraphjni`, etc.

### App Native Libraries
- `consolenative` (specific to `consoleapp`)

**Note:** JNI libraries like `basejni`, `mediajni`, `networkjni`, etc. are **Native Libraries** (not App Native Libraries). They are core libraries that provide JNI bindings and must be built as static libraries so other projects can link against them.

### Java Apps
- `consoleapp`
