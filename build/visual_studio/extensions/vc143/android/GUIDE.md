# Ocean Android Extension - Installation & Usage Guide

A comprehensive Visual Studio 2022 extension for Android development with first-class native C++ support.

---

## Table of Contents

- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Configuration](#configuration)
- [Quick Start](#quick-start)
- [Project Types](#project-types)
- [Building](#building)
- [Debugging](#debugging)
- [Tool Windows](#tool-windows)
- [Native C++ Integration](#native-c-integration)
- [Command Line Usage](#command-line-usage)
- [Troubleshooting](#troubleshooting)

---

## Prerequisites

Before installing Ocean Android Extension, ensure you have the following:

| Requirement | Minimum Version | Download |
|-------------|-----------------|----------|
| Visual Studio 2022 | 17.0+ | [visualstudio.microsoft.com](https://visualstudio.microsoft.com/) |
| Android SDK | Latest | Via Android Studio or SDK Manager |
| JDK | 17+ | [Adoptium](https://adoptium.net/) or [Oracle](https://www.oracle.com/java/) |
| Android NDK | 25+ (optional) | For native C++ development (auto-detected from SDK) |

### Installing JDK 17

A JDK 17-21 installation is required. The project auto-detects JDK installations from common locations.

**Option 1: Microsoft OpenJDK (recommended)**
```cmd
winget install --id Microsoft.OpenJDK.17 -e
```
Installs to `C:\Program Files\Microsoft\jdk-17...` (auto-detected by the project).

**Option 2: Eclipse Adoptium (Temurin)**
```cmd
winget install --id EclipseAdoptium.Temurin.17.JDK -e
```
Installs to `C:\Program Files\Eclipse Adoptium\jdk-17...` (auto-detected by the project).

> **Note:** The Android Studio bundled JBR may cause `jlink.exe` errors during builds. Installing a standalone JDK 17 avoids this issue. The project auto-detects standalone JDK installations with higher priority than the Android Studio JBR.

### Installing Android SDK (if not already installed)

**Option 1: Via Android Studio**
1. Download [Android Studio](https://developer.android.com/studio)
2. Run the installer - SDK installs automatically
3. Default location: `%LOCALAPPDATA%\Android\Sdk`

**Option 2: Command Line Tools Only**
1. Download [Command Line Tools](https://developer.android.com/studio#command-tools)
2. Extract to a folder (e.g., `C:\Android\cmdline-tools`)
3. Run: `sdkmanager "platform-tools" "build-tools;34.0.0" "platforms;android-34"`

---

## Installation

### Building from Source

**Prerequisites:**

To build the extension from source, you need:
- **Visual Studio 2022** with the **Visual Studio extension development** workload
  - Install via: Visual Studio Installer → Modify → Workloads → Visual Studio extension development
  - This workload includes the Visual Studio SDK and required extension development components
- **.NET Framework 4.8 Developer Pack** (includes reference assemblies needed for compilation)

**Build Steps:**

```bash
# Navigate to the extension directory
cd xplat\ocean\build\visual_studio\extensions\vc143\android

# Open in Visual Studio
start OceanAndroidExtension.sln
```

1. Set configuration to **Release**
2. Build the solution (`Ctrl+Shift+B`)
3. Find the output: `bin\extensions\android\release\OceanAndroidExtension.vsix`

### Installing the Extension

**Method 1: Double-click**
- Navigate to the `.vsix` file and double-click

**Method 2: Via Visual Studio**
1. Extensions → Manage Extensions
2. Click "Install from VSIX..."
3. Select `OceanAndroidExtension.vsix`

**Method 3: VSIX Installer**
```bash
VSIXInstaller.exe OceanAndroidExtension.vsix
```

> ⚠️ **Restart Visual Studio** after installation

### Installing the OceanNDK Application Type

For native C++ Android projects, you must install the OceanNDK Application Type into Visual Studio's MSBuild directories. This enables the custom property pages and build rules.

1. Open PowerShell **as Administrator**
2. Run:
   ```powershell
   cd xplat\ocean\build\visual_studio\extensions\vc143\android\scripts
   .\InstallOceanNDKToolset.ps1
   ```

This script:
- Detects all installed Visual Studio versions (2019, 2022, etc.)
- Copies the OceanNDK Application Type to `$(VCTargetsPath)\Application Type\OceanNDK\1.0\`
- Enables projects with `<ApplicationType>OceanNDK</ApplicationType>` to load properly

To uninstall:
```powershell
.\InstallOceanNDKToolset.ps1 -Uninstall
```

---

## Configuration

### SDK Path Setup

After installation, configure your SDK paths:

1. Navigate to **Tools → Options → Android → SDK Paths**

2. Configure the following:

| Setting | Example Path | Environment Variable |
|---------|--------------|----------------------|
| Android SDK | `C:\Users\You\AppData\Local\Android\Sdk` | `ANDROID_HOME` |
| Android NDK | Auto-detected from SDK/ndk/ folder | `ANDROID_NDK_HOME` (optional) |
| JDK | `C:\Program Files\Eclipse Adoptium\jdk-17` | `JAVA_HOME` or `OCEAN_JAVA_HOME` |

> 💡 **Tip:** The NDK is automatically detected from your SDK installation. No manual configuration required!

### NDK Path Resolution

The toolset finds the Android NDK using the following priority:

| Priority | Source | Description |
|----------|--------|-------------|
| 0 | `OCEAN_ANDROID_NDK_HOME` | Ocean-specific override for using a custom NDK folder |
| 1 | `ANDROID_NDK_HOME` / `ANDROID_NDK_ROOT` / `NDK_ROOT` | Standard environment variables |
| 2 | Registry | Auto-detected by extension from SDK/ndk/ folder |
| 3 | `ndk-bundle` | Legacy SDK installations |

#### Using `OCEAN_ANDROID_NDK_HOME`

The `OCEAN_ANDROID_NDK_HOME` environment variable allows you to specify a custom NDK folder specifically for Ocean Android projects. This is useful when:

- **`ANDROID_NDK_HOME` is already in use by another application** (e.g., Android Studio, Unity, Unreal Engine) and points to a different NDK version
- You need to **pin a specific NDK version** for Ocean builds without affecting other Android development tools
- You want to **test with a different NDK version** temporarily without changing system-wide settings

**Example - Using a custom NDK folder:**
```cmd
:: Set Ocean to use a specific NDK version while ANDROID_NDK_HOME points elsewhere
set OCEAN_ANDROID_NDK_HOME=C:\Android\CustomNDK\ndk\26.1.10909125

:: ANDROID_NDK_HOME can still point to another NDK for other applications
set ANDROID_NDK_HOME=C:\Users\You\AppData\Local\Android\Sdk\ndk\25.2.9519653
```

**Example - Permanent configuration (System Environment Variables):**
1. Open **System Properties** → **Advanced** → **Environment Variables**
2. Add a new **User variable**:
   - Name: `OCEAN_ANDROID_NDK_HOME`
   - Value: `C:\Path\To\Your\Custom\NDK`

> 💡 **Tip:** If `OCEAN_ANDROID_NDK_HOME` is set, it takes the highest priority and will always be used, regardless of other NDK environment variables or registry settings.

### JDK Path Resolution

The project finds the JDK using the following priority:

| Priority | Source | Description |
|----------|--------|-------------|
| 0 | `OCEAN_JAVA_HOME` | Ocean-specific override for using a custom JDK |
| 1 | Auto-detect JDK 17-21 | Scans standard install locations (Program Files) |
| 2 | Android Studio JBR | Bundled Java Runtime from Android Studio |
| 3 | `JAVA_HOME` | Standard environment variable (may be incompatible version) |

#### Using `OCEAN_JAVA_HOME`

The `OCEAN_JAVA_HOME` environment variable allows you to specify a custom JDK specifically for Ocean Android projects. This is useful when:

- **`JAVA_HOME` is already in use by another application** (e.g., a server, IDE, or other build system) and points to a different JDK version
- You need to **pin a specific JDK version** for Ocean builds without affecting other Java development tools
- You want to **test with a different JDK version** temporarily without changing system-wide settings
- Your auto-detected JDK is the **Android Studio JBR**, which can cause `jlink.exe` errors

**Example - Using a custom JDK:**
```cmd
:: Set Ocean to use a specific JDK while JAVA_HOME points elsewhere
set OCEAN_JAVA_HOME=C:\Program Files\Eclipse Adoptium\jdk-17.0.11.9-hotspot

:: JAVA_HOME can still point to another JDK for other applications
set JAVA_HOME=C:\Program Files\Java\jdk-11
```

**Example - Permanent configuration (System Environment Variables):**
1. Open **System Properties** → **Advanced** → **Environment Variables**
2. Add a new **User variable**:
   - Name: `OCEAN_JAVA_HOME`
   - Value: `C:\Path\To\Your\JDK17`

> **Tip:** If `OCEAN_JAVA_HOME` is set, it takes the highest priority and will always be used, regardless of auto-detection or `JAVA_HOME`. The build output will show which JDK is being used:
> ```
> [OceanAndroid] Java Home: C:\Program Files\Eclipse Adoptium\jdk-17.0.11.9-hotspot
> [OceanAndroid] Java Version: 17.0.11 (JDK 17)
> ```

### Recommended Settings

| Setting | Recommended Value | Description |
|---------|-------------------|-------------|
| Default Min SDK | 24 | Android 7.0 - covers 97%+ of devices |
| Default Target SDK | 34 | Latest stable API level |
| Enable Native Debugging | true | For C++ development |
| Use Hardware Acceleration | true | Faster emulator performance |

---

## Quick Start

### Your First Android App in 5 Steps

**Step 1: Create Project**
```
File → New → Project → Search "Ocean Android Application"
```

**Step 2: Configure**
- Project name: `MyFirstApp`
- Package name: `com.example.myfirstapp`
- Min SDK: 24

**Step 3: Build**
```
Ctrl+Shift+B
```

**Step 4: Connect Device**
- Enable USB debugging on your Android device, or
- Start an emulator via Device Explorer

**Step 5: Run**
```
F5
```

🎉 Your app is now running!

---

## Project Types

### Ocean Android Application (.oceanandroidproj)

Full Android application with Gradle build system.

**Features:**
- Java and Kotlin source files
- Android resources (layouts, drawables, strings)
- Gradle-based builds (assembleDebug/Release)
- Project references to native libraries
- APK and AAB output

**Project Structure:**
```
MyApp/
├── MyApp.oceanandroidproj     # VS project file
├── app/
│   ├── build.gradle.kts       # Module build config
│   └── src/main/
│       ├── AndroidManifest.xml
│       ├── java/              # Java/Kotlin sources
│       └── res/               # Resources
├── build.gradle.kts           # Root build config
├── settings.gradle.kts
├── gradle.properties
├── gradlew
└── gradlew.bat
```

---

### Android Native Library (.vcxproj with OceanNDK Application Type)

C++ shared library targeting Android NDK using the OceanNDK MSBuild toolset.

**Features:**
- Direct clang/LLVM compilation (no CMake required)
- Multi-ABI support via VS Platform configurations (ARM64, ARM, x64, x86)
- Full Visual Studio IntelliSense
- Automatic NDK detection from SDK folder
- Incremental builds with .tlog tracking

**Project Structure:**
```
MyNativeLib/
├── MyNativeLib.vcxproj        # VS C++ project file
└── src/
    ├── native-lib.cpp
    └── native-lib.h
```

**Key .vcxproj Settings:**
```xml
<PropertyGroup Label="Globals">
  <ProjectGuid>{YOUR-GUID-HERE}</ProjectGuid>
  <RootNamespace>MyNativeLib</RootNamespace>
  <Keyword>Android</Keyword>

  <!-- OceanNDK Application Type - enables Android property pages -->
  <ApplicationType>OceanNDK</ApplicationType>
  <ApplicationTypeRevision>1.0</ApplicationTypeRevision>

  <IsAndroidNdkProject>true</IsAndroidNdkProject>
</PropertyGroup>
```

**Platform to ABI Mapping:**

| VS Platform | Android ABI | Architecture |
|-------------|-------------|--------------|
| ARM64 | arm64-v8a | 64-bit ARM (modern phones) |
| ARM | armeabi-v7a | 32-bit ARM (older phones) |
| x64 | x86_64 | 64-bit x86 (emulators) |
| x86 | x86 | 32-bit x86 (older emulators) |

---

### Android Library

Reusable Android library module (AAR output).

---

## Building

### Build Commands

| Action | Keyboard | Menu |
|--------|----------|------|
| Build Solution | `Ctrl+Shift+B` | Build → Build Solution |
| Build Project | `Ctrl+B` | Build → Build [Project] |
| Rebuild | - | Build → Rebuild Solution |
| Clean | - | Build → Clean Solution |

### Build Configurations

| Configuration | Gradle Task | Output |
|---------------|-------------|--------|
| Debug | `assembleDebug` | `app-debug.apk` |
| Release | `assembleRelease` | `app-release.apk` |

### Build Output Location

```
MyApp/app/build/outputs/apk/debug/app-debug.apk
MyApp/app/build/outputs/apk/release/app-release.apk
```

### Customizing Builds

Edit project properties or `build.gradle.kts`:

```kotlin
android {
    defaultConfig {
        minSdk = 24
        targetSdk = 34
        versionCode = 1
        versionName = "1.0"
    }
}
```

---

## Debugging

### Java/Kotlin Debugging

1. Set breakpoints by clicking the left margin in `.java` or `.kt` files
2. Press `F5` to start debugging
3. Use standard VS debugging tools:
   - Step Over (`F10`)
   - Step Into (`F11`)
   - Continue (`F5`)
   - Watch windows, Call Stack, etc.

### Native C++ Debugging

**Enable Native Debugging:**
1. Right-click project → Properties
2. Set **Enable Native Debugging** = `true`

**Debug Native Code:**
1. Set breakpoints in `.cpp` files
2. Press `F5` - both Java and native debuggers attach
3. Step through native code normally

### Mixed-Mode Debugging

With native debugging enabled, you can:
- Set breakpoints in both Java and C++ code
- Step from Java into JNI calls
- View native call stack alongside Java frames

### Debug Without Deploy

To attach to an already-running app:
1. Debug → Attach to Process
2. Select your device
3. Choose the app process

---

## Tool Windows

Access via **View → Android** or the keyboard shortcuts below.

### Device Explorer

**Purpose:** Manage connected devices and emulators

**Features:**
- View all connected devices
- Device properties (model, API level, serial)
- Start/stop emulators
- Install APKs via drag-and-drop
- Device shell access

**Usage:**
```
View → Android → Device Explorer
```

---

### Logcat

**Purpose:** Real-time Android log viewer

**Features:**
- Live log streaming from device
- Filter by:
  - Log level (Verbose, Debug, Info, Warning, Error)
  - Tag name
  - Search text
- Clear and export logs

**Usage:**
```
View → Android → Logcat
```

**Log Levels:**
| Level | Color | Description |
|-------|-------|-------------|
| V | Gray | Verbose - detailed tracing |
| D | Cyan | Debug - debugging messages |
| I | Green | Info - informational messages |
| W | Orange | Warning - potential issues |
| E | Red | Error - errors and exceptions |

---

### SDK Manager

**Purpose:** View and manage SDK components

**Features:**
- List installed platforms, build-tools, NDK versions
- View component paths
- Quick access to Android SDK Manager

**Usage:**
```
View → Android → SDK Manager
```

---

## Native C++ Integration

The key feature of Ocean Android Extension is seamless C++ integration using the OceanNDK Application Type and MSBuild toolset.

### How It Works

```
┌─────────────────────────────────────────────────────────┐
│                    Build Process                        │
├─────────────────────────────────────────────────────────┤
│  1. Native project builds (.vcxproj with OceanNDK)      │
│     └─→ Uses NDK clang directly via MSBuild             │
│     └─→ Outputs: lib*.so for selected platform/ABI     │
│                                                         │
│  2. Android project detects reference                   │
│     └─→ Copies .so files to jniLibs/<abi>/              │
│                                                         │
│  3. Gradle packages everything                          │
│     └─→ APK includes native libraries                   │
└─────────────────────────────────────────────────────────┘
```

### The OceanNDK Application Type

The extension provides a custom MSBuild Application Type that:
- Uses NDK's clang++ compiler directly (no CMake wrapper)
- Maps VS platforms (ARM64, ARM, x64, x86) to Android ABIs
- Provides proper IntelliSense with NDK headers
- Supports incremental builds with .tlog files
- Auto-detects NDK from your SDK installation

**Note:** The Platform Toolset remains `v143` (or your default MSVC toolset). The Application Type provides the Android-specific behavior.

### Adding a Native Reference

1. **Create Native Library Project**
   ```
   Right-click Solution → Add → New Project → Android Native Library
   ```

2. **Add Reference from Android App**
   ```
   Right-click Android Project → Add → Project Reference → Select Native Project
   ```

3. **Build**
   - Native libraries build first
   - `.so` files automatically copy to `app/src/main/jniLibs/`
   - APK includes all native code

### Calling Native Code from Java/Kotlin

**Java Side:**
```java
public class NativeLib {
    static {
        System.loadLibrary("mynativelib");
    }

    public native String getVersionString();
    public native int add(int a, int b);
}
```

**C++ Side:**
```cpp
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapp_NativeLib_getVersionString(JNIEnv* env, jobject) {
    return env->NewStringUTF("1.0.0");
}

extern "C" JNIEXPORT jint JNICALL
Java_com_example_myapp_NativeLib_add(JNIEnv* env, jobject, jint a, jint b) {
    return a + b;
}
```

### Supported ABIs

| ABI | Architecture | Devices |
|-----|--------------|---------|
| `arm64-v8a` | 64-bit ARM | Modern phones (2016+) |
| `armeabi-v7a` | 32-bit ARM | Older phones |
| `x86_64` | 64-bit x86 | Emulators, Chromebooks |
| `x86` | 32-bit x86 | Older emulators |

---

## Command Line Usage

### MSBuild Commands

```bash
# Build debug
msbuild MyApp.oceanandroidproj /p:Configuration=Debug

# Build release
msbuild MyApp.oceanandroidproj /p:Configuration=Release

# Deploy to device
msbuild MyApp.oceanandroidproj /t:Deploy

# Clean build
msbuild MyApp.oceanandroidproj /t:Clean

# Rebuild
msbuild MyApp.oceanandroidproj /t:Rebuild
```

### Building Native Libraries

```bash
# Build for ARM64 (arm64-v8a)
msbuild MyNativeLib.vcxproj /p:Configuration=Release /p:Platform=ARM64

# Build for all ABIs
msbuild MyNativeLib.vcxproj /t:BuildAllAbis /p:Configuration=Release

# Single file compile (like Ctrl+F7)
msbuild MyNativeLib.vcxproj /t:CompileSingleFile /p:SelectedFiles=src\native-lib.cpp
```

### Gradle Commands (Direct)

```bash
# Navigate to project directory
cd MyApp

# Build debug APK
gradlew assembleDebug

# Build release APK
gradlew assembleRelease

# Run tests
gradlew test

# Clean
gradlew clean
```

### ADB Commands

```bash
# List devices
adb devices

# Install APK
adb install -r app-debug.apk

# Launch app
adb shell am start -n com.example.myapp/.MainActivity

# View logs
adb logcat

# Uninstall
adb uninstall com.example.myapp
```

---

## Troubleshooting

### Common Issues

#### "Android SDK not found"

**Solution:**
1. Install Android SDK via Android Studio or standalone tools
2. Set `ANDROID_HOME` environment variable
3. Configure path in Tools → Options → Android → SDK Paths

#### "Android NDK not found"

**Solution:**
The NDK is auto-detected from your SDK installation. If it's not found:
1. Ensure NDK is installed in `SDK/ndk/<version>/` folder
2. Open Visual Studio at least once (extension writes registry on startup)
3. Or set `ANDROID_NDK_HOME` environment variable manually
4. To pin a specific version: set `OCEAN_ANDROID_NDK_HOME`

**Check the build output for diagnostics:**
```
[OceanNDK] NDK Path Resolution:
[OceanNDK]   OCEAN_ANDROID_NDK_HOME   =
[OceanNDK]   ANDROID_NDK_HOME env     =
[OceanNDK]   Registry NdkPath         = C:\Users\...\Sdk\ndk\26.1.10909125\
[OceanNDK]   AndroidNdkPath (final)   = C:\Users\...\Sdk\ndk\26.1.10909125\
```

#### "The target 'X' does not exist in the project" or "Cannot load project"

**Solution:**
This usually means the OceanNDK Application Type is not installed. Run:
```powershell
# Run as Administrator
cd xplat\ocean\build\visual_studio\extensions\vc143\android\scripts
.\InstallOceanNDKToolset.ps1
```

#### "gradle-wrapper.jar integrity check FAILED"

The build verifies `gradle-wrapper.jar` against a SHA-256 checksum before invoking Gradle. If you see this error, the JAR has been modified or replaced with an unexpected version.

**Expected hash for Gradle 8.5:**
```
d3b261c2820e9e3d8d639ed084900f11f4a86050a8f83342ade7b6bc9b0d2bdd
```
Source: https://services.gradle.org/distributions/gradle-8.5-bin.zip

**Solution:**
1. Re-download the correct `gradle-wrapper.jar` from the [Gradle GitHub repository](https://raw.githubusercontent.com/gradle/gradle/v8.5.0/gradle/wrapper/gradle-wrapper.jar)
2. Replace `gradle/wrapper/gradle-wrapper.jar` in your project
3. Verify the SHA-256 hash matches the value above

If you are intentionally upgrading Gradle, update both the JAR and the hash in the extension's `toolset\GradleWrapper\gradle-wrapper.jar.sha256`.

#### "Gradle build failed"

**Check:**
- JDK is installed and `JAVA_HOME` is set
- Internet connection (Gradle downloads dependencies)
- Gradle wrapper exists (`gradlew.bat`)

**Try:**
```bash
gradlew --stacktrace assembleDebug
```

#### "JdkImageTransform" or "jlink.exe" error during build

This error typically looks like:
```
Failed to transform core-for-system-modules.jar
Error while executing process ...\jlink.exe
```

**Cause:** The Android Studio bundled JBR (Java Runtime) is incompatible with the Gradle/AGP version being used.

**Solution:** Install a standalone JDK 17:
```cmd
winget install --id Microsoft.OpenJDK.17 -e
```

The project auto-detects standalone JDK installations with higher priority than the Android Studio JBR. If the issue persists, also clear the Gradle transform cache:
```cmd
rd /s /q "%USERPROFILE%\.gradle\caches\transforms-3"
```

#### "No devices found"

**For Physical Devices:**
1. Enable Developer Options on device
2. Enable USB Debugging
3. Trust computer when prompted
4. Check USB cable

**For Emulators:**
1. Open Device Explorer
2. Click "Start Emulator"
3. Wait for boot to complete

#### "Native library not loading"

**Check:**
- Library name matches `System.loadLibrary()` call
- All ABIs are built
- `.so` files are in `jniLibs/<abi>/`

#### Build is slow

**Solutions:**
- Enable Gradle daemon (default)
- Enable parallel builds: `org.gradle.parallel=true`
- Increase memory: `org.gradle.jvmargs=-Xmx4g`
- Use configuration cache: `org.gradle.configuration-cache=true`

---

## Keyboard Shortcuts Reference

| Action | Shortcut |
|--------|----------|
| Build Solution | `Ctrl+Shift+B` |
| Start Debugging | `F5` |
| Start Without Debugging | `Ctrl+F5` |
| Stop Debugging | `Shift+F5` |
| Step Over | `F10` |
| Step Into | `F11` |
| Step Out | `Shift+F11` |
| Toggle Breakpoint | `F9` |
| Go to Definition | `F12` |
| Find All References | `Shift+F12` |
| Compile Single File | `Ctrl+F7` |

---

## Getting Help

- **Documentation:** See `README.md` in this directory
- **Build Output:** Check Output window (Build) for detailed diagnostics
- **Debug Log:** `%LOCALAPPDATA%\OceanAndroidExtension\debug.log`

---

*Ocean Android Extension is part of the Ocean project. Licensed under the MIT License.*
