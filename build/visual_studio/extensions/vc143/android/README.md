# Ocean Android Extension

A Visual Studio 2022 extension enabling Android app development with first-class native C++ library support.

## Features

- **Build & Run** - Gradle-based builds, APK packaging, device/emulator deployment
- **Project Templates** - Android Application, Library, and Native C++ projects
- **Debugging** - Java/Kotlin and native C++ debugging via ADB/LLDB
- **IntelliSense** - Code completion for Android APIs and NDK
- **Native C++ Integration** - Manage NDK libraries as VS projects with MSBuild-based toolset
- **Tool Windows** - Device Explorer, Logcat viewer, SDK Manager

## Requirements

- Visual Studio 2022 (17.0 or later)
- Android SDK (with platform-tools and build-tools)
- Android NDK (for native C++ development) - auto-detected from SDK folder
- JDK 17 to 24

## Installation

1. Build the extension from source (see [Building from Source](#building-from-source))
2. Double-click the `.vsix` file to install, or use Extensions > Manage Extensions in VS
3. Restart Visual Studio
4. Configure SDK paths in Tools > Options > Android > SDK Paths

### OceanNDK Application Type Installation

For native C++ Android projects, you need to install the OceanNDK Application Type into Visual Studio's MSBuild directories:

1. Open PowerShell **as Administrator**
2. Run the installation script:
   ```powershell
   cd xplat\ocean\build\visual_studio\extensions\vc143\android\scripts
   .\InstallOceanNDKToolset.ps1
   ```

This copies the OceanNDK Application Type files to Visual Studio, enabling projects to use:
```xml
<ApplicationType>OceanNDK</ApplicationType>
<ApplicationTypeRevision>1.0</ApplicationTypeRevision>
```

The `PlatformToolset` is set to `OceanNDK` - the Application Type provides the Android-specific property pages and build rules.

To uninstall the Application Type:
```powershell
.\InstallOceanNDKToolset.ps1 -Uninstall
```

## Quick Start

### Create an Android Application

1. File > New > Project
2. Search for "Ocean Android Application"
3. Configure project name and package name
4. Build with Ctrl+Shift+B
5. Deploy with F5

### Add Native C++ Library

1. Right-click Solution > Add > New Project
2. Select "Android Native Library (C++)"
3. Add project reference from Android app to native library
4. Build - native .so files are automatically copied to jniLibs

## Project Types

### Ocean Android Application (.vcxproj with Makefile ConfigurationType)

Android app with Gradle build system. Supports:
- Java and Kotlin source files
- Resource files (layouts, drawables, strings)
- Project references to native libraries
- Gradle-based builds (assembleDebug/assembleRelease)

### Android Native Library (.vcxproj with OceanNDK Application Type)

C++ shared library targeting Android NDK using the OceanNDK MSBuild toolset. Features:
- Direct clang/LLVM compilation without CMake
- Multi-ABI support via VS Platform configurations (ARM64, ARM, x64, x86)
- Full VS IntelliSense with NDK headers
- Automatic NDK detection from SDK folder
- Incremental builds with proper .tlog tracking

**Key project file settings:**
```xml
<PropertyGroup Label="Globals">
  <ApplicationType>OceanNDK</ApplicationType>
  <ApplicationTypeRevision>1.0</ApplicationTypeRevision>
  <IsAndroidNdkProject>true</IsAndroidNdkProject>
</PropertyGroup>
```

## NDK Path Resolution

The toolset automatically detects the Android NDK using the following priority:

| Priority | Source | Description |
|----------|--------|-------------|
| 0 | `OCEAN_ANDROID_NDK_HOME` | Override for pinning a specific NDK version |
| 1 | `ANDROID_NDK_HOME` | Standard environment variable |
| 2 | `ANDROID_NDK_ROOT` / `NDK_ROOT` | Alternative standard environment variables |
| 3 | Registry | Auto-detected by extension from SDK/ndk/ folder |
| 4 | `ndk-bundle` | Legacy SDK installations |

To pin a specific NDK version (e.g., for compatibility):
```cmd
set OCEAN_ANDROID_NDK_HOME=C:\Users\you\AppData\Local\Android\Sdk\ndk\25.2.9519653
```

## JDK Path Resolution

The project automatically detects the JDK using the following priority:

| Priority | Source | Description |
|----------|--------|-------------|
| 0 | `OCEAN_JAVA_HOME` | Override for pinning a specific JDK for Ocean builds |
| 1 | Auto-detect JDK 17-24 | Scans standard install locations (Program Files) |
| 2 | Android Studio JBR | Bundled Java Runtime from Android Studio |
| 3 | `JAVA_HOME` | Standard environment variable (may be incompatible version) |

To pin a specific JDK (e.g., when `JAVA_HOME` points to an incompatible version):
```cmd
set OCEAN_JAVA_HOME=C:\Program Files\Eclipse Adoptium\jdk-17.0.11.9-hotspot
```

## Native Integration

When an Android Application project references a Native Library project:

1. Native project builds first (produces .so for each ABI)
2. .so files are copied to `app/src/main/jniLibs/<abi>/`
3. Gradle includes them in the APK automatically

```xml
<!-- Example project reference in .vcxproj -->
<ProjectReference Include="..\NativeLib\NativeLib.vcxproj">
  <Project>{GUID}</Project>
  <CopyToJniLibs>true</CopyToJniLibs>
</ProjectReference>
```

## Debugging

### Java/Kotlin Debugging
- Set breakpoints in Java/Kotlin files
- Press F5 to deploy and debug
- Uses JDWP (Java Debug Wire Protocol)

### Native C++ Debugging
- Enable in project properties: "Enable Native Debugging = true"
- Set breakpoints in C++ files
- Uses LLDB from Android NDK
- Supports mixed-mode (Java + native) debugging

## Tool Windows

### Device Explorer (View > Android > Device Explorer)
- View connected devices and emulators
- Device properties and shell access
- Install APKs and browse files

### Logcat (View > Android > Logcat)
- Real-time log streaming
- Filter by app, log level, or tag
- Search and export logs

### SDK Manager (View > Android > SDK Manager)
- View installed SDK components
- Open Android SDK Manager

## MSBuild Properties

### Android Application
```xml
<PropertyGroup>
  <AndroidMinSdkVersion>24</AndroidMinSdkVersion>
  <AndroidTargetSdkVersion>34</AndroidTargetSdkVersion>
  <AndroidBuildType>debug</AndroidBuildType>
  <EnableNativeDebugging>true</EnableNativeDebugging>
</PropertyGroup>
```

### Native Library (OceanNDK Application Type)
```xml
<PropertyGroup>
  <!-- API level (Android 7.0+) -->
  <AndroidApiLevel>24</AndroidApiLevel>
  <!-- STL implementation -->
  <AndroidStl>c++_shared</AndroidStl>
  <!-- C++ standard -->
  <AndroidCppStandard>c++17</AndroidCppStandard>
</PropertyGroup>

<ItemDefinitionGroup>
  <ClCompile>
    <LanguageStandard>c++17</LanguageStandard>
    <RuntimeTypeInfo>true</RuntimeTypeInfo>
    <ExceptionHandling>true</ExceptionHandling>
  </ClCompile>
</ItemDefinitionGroup>
```

### Platform to ABI Mapping

| VS Platform | Android ABI | Architecture |
|-------------|-------------|--------------|
| ARM64 | arm64-v8a | 64-bit ARM |
| ARM | armeabi-v7a | 32-bit ARM |
| x64 | x86_64 | 64-bit x86 |
| x86 | x86 | 32-bit x86 |

## Building from Source

### Prerequisites for Building the Extension

To build this extension from source, you need:

- **Visual Studio 2022** with the **Visual Studio extension development** workload installed
  - This workload includes the Visual Studio SDK and required components
  - Install via Visual Studio Installer → Modify → Workloads → Visual Studio extension development
- **.NET Framework 4.8 Developer Pack** (includes reference assemblies for compilation)

### Build Steps

```bash
# Navigate to the extension directory
cd xplat\ocean\build\visual_studio\extensions\vc143\android

# Open in Visual Studio
start OceanAndroidExtension.sln

# Build solution (Release configuration recommended)
msbuild OceanAndroidExtension.sln /p:Configuration=Release

# Output VSIX is in bin\extensions\android\release\
```

## Running Tests

```bash
# Unit tests
dotnet test tests\OceanAndroidExtension.Tests\OceanAndroidExtension.Tests.csproj

# Integration tests (requires Android SDK)
dotnet test tests\Integration.Tests\Integration.Tests.csproj
```

## Project Structure

```
xplat\ocean\build\visual_studio\extensions\vc143\android\
├── OceanAndroidExtension.sln          # Solution file
├── Directory.Build.props              # Shared MSBuild properties
├── Directory.Packages.props           # Central NuGet package versions
├── src\
│   ├── OceanAndroidExtension\             # Main VSIX extension package
│   ├── OceanAndroidExtension.BuildTasks\  # Custom MSBuild tasks
│   └── OceanAndroidExtension.LanguageService\  # Java/Kotlin language services
├── toolset\                        # MSBuild props/targets for OceanNDK
├── apptype\                        # OceanNDK Application Type files
├── templates\                      # VS project templates
├── tests\                          # Unit and integration tests
└── scripts\                        # Installation scripts
```

## Gradle Wrapper Integrity

The project templates include `gradle-wrapper.jar`, a bootstrap JAR that downloads the Gradle distribution. Both templates ship the same JAR verified against a `.sha256` checksum file.

| Property | Value |
|----------|-------|
| Gradle version | 8.5 |
| Source | https://services.gradle.org/distributions/gradle-8.5-bin.zip |
| SHA-256 | `d3b261c2820e9e3d8d639ed084900f11f4a86050a8f83342ade7b6bc9b0d2bdd` |

The `VerifyGradleWrapper` MSBuild target runs automatically before each build and compares the JAR against the expected hash stored in the extension's `toolset\GradleWrapper\gradle-wrapper.jar.sha256`. The checksum file is part of the extension, not copied into each project. If the hash does not match, the build fails with an error. If the `.sha256` file is missing, a warning is logged but the build continues.

To update the wrapper JAR (e.g., when upgrading Gradle), replace `gradle-wrapper.jar` in both template directories and update `toolset\GradleWrapper\gradle-wrapper.jar.sha256` with the new hash.

## License

MIT License - see [LICENSE.txt](LICENSE.txt) for details.

## Acknowledgments

- Android SDK and NDK by Google
- Eclipse JDT Language Server for Java IntelliSense
- Kotlin Language Server for Kotlin IntelliSense
