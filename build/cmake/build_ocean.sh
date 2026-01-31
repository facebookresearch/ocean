#!/usr/bin/env bash
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# Unified build script for Ocean using CMake Presets.
# Supports native builds and cross-compilation for all platforms.

set -e

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
OCEAN_SOURCE_DIR=$( cd "${SCRIPT_DIR}" && cd ../.. && pwd )

# Detect host platform
detect_host_platform() {
    case "$(uname -s)" in
        Darwin) echo "macos" ;;
        Linux) echo "linux" ;;
        MINGW*|MSYS*|CYGWIN*) echo "windows" ;;
        *) echo "unknown" ;;
    esac
}

# Detect host architecture
detect_host_arch() {
    case "$(uname -m)" in
        arm64|aarch64) echo "arm64" ;;
        x86_64|amd64) echo "x64" ;;
        *) echo "$(uname -m)" ;;
    esac
}

HOST_PLATFORM=$(detect_host_platform)
HOST_ARCH=$(detect_host_arch)

# Defaults
TARGET_PLATFORM=""  # Will be set to host platform if not specified
OCEAN_CONFIGS="debug,release"
OCEAN_LINK_TYPES="static"
OCEAN_ARCH=""  # Will be auto-detected or set based on target
OCEAN_BUILD_DIR="${PWD}/ocean_build"
OCEAN_INSTALL_DIR="${PWD}/ocean_install"
OCEAN_THIRD_PARTY_DIR="${PWD}/ocean_install_thirdparty"
OCEAN_PARALLEL="ON"
OCEAN_INSTALL_TARGET="ON"
OCEAN_QUEST_MODE="OFF"
OCEAN_MINIMAL="OFF"
OCEAN_LOG_LEVEL="ERROR"
OCEAN_ANDROID_SDK="android-32"
OCEAN_GENERATOR=""  # Empty means auto-detect/use preset default

# Valid log levels
OCEAN_VALID_LOG_LEVELS="ERROR,WARNING,NOTICE,STATUS,VERBOSE,DEBUG,TRACE"

# Check CMake is available and supports presets
check_cmake() {
    if ! command -v cmake &> /dev/null; then
        echo "ERROR: cmake is not installed or not in your PATH." >&2
        case "${HOST_PLATFORM}" in
            macos)
                echo "  Install via Homebrew: brew install cmake" >&2
                ;;
            linux)
                echo "  Ubuntu/Debian: sudo apt-get install cmake" >&2
                echo "  Fedora/RHEL: sudo dnf install cmake" >&2
                ;;
        esac
        exit 1
    fi

    local cmake_version=$(cmake --version | head -1 | cut -d' ' -f3)
    local cmake_major=$(echo "$cmake_version" | cut -d'.' -f1)
    local cmake_minor=$(echo "$cmake_version" | cut -d'.' -f2)

    if [[ "$cmake_major" -lt 3 ]] || { [[ "$cmake_major" -eq 3 ]] && [[ "$cmake_minor" -lt 25 ]]; }; then
        echo "ERROR: CMake 3.25 or later is required for preset support." >&2
        echo "  Current version: $cmake_version" >&2
        exit 1
    fi
}

# Validate cross-compilation is possible
validate_cross_compile() {
    local target=$1

    case "${HOST_PLATFORM}:${target}" in
        macos:macos|macos:ios|macos:android) return 0 ;;
        linux:linux|linux:android) return 0 ;;
        windows:windows|windows:android) return 0 ;;
        *)
            echo "ERROR: Cannot build for '${target}' on '${HOST_PLATFORM}' host." >&2
            echo "" >&2
            echo "Supported combinations:" >&2
            echo "  macOS host   -> macos, ios, android" >&2
            echo "  Linux host   -> linux, android" >&2
            echo "  Windows host -> windows, android" >&2
            exit 1
            ;;
    esac
}

# Check Android prerequisites
check_android_prerequisites() {
    if [[ -z "${ANDROID_NDK}" ]]; then
        echo "ERROR: ANDROID_NDK environment variable is not set." >&2
        echo "  Set it to the location of your Android NDK installation." >&2
        echo "  Example: export ANDROID_NDK=/path/to/android-ndk-r25c" >&2
        exit 1
    fi

    if [[ ! -d "${ANDROID_NDK}" ]]; then
        echo "ERROR: ANDROID_NDK directory does not exist: ${ANDROID_NDK}" >&2
        exit 1
    fi

    if [[ ! -f "${ANDROID_NDK}/build/cmake/android.toolchain.cmake" ]]; then
        echo "ERROR: Android toolchain not found in NDK: ${ANDROID_NDK}" >&2
        exit 1
    fi

    if [[ -z "${JAVA_HOME}" ]]; then
        echo "ERROR: JAVA_HOME environment variable is not set." >&2
        echo "  Set it to the location of your Java installation." >&2
        exit 1
    fi
}

# Check iOS prerequisites
check_ios_prerequisites() {
    if [[ "${HOST_PLATFORM}" != "macos" ]]; then
        echo "ERROR: iOS builds require macOS host." >&2
        exit 1
    fi

    if ! command -v xcodebuild &> /dev/null; then
        echo "ERROR: Xcode is required for iOS builds." >&2
        echo "  Install Xcode from the App Store." >&2
        exit 1
    fi
}

# Check Windows prerequisites
check_windows_prerequisites() {
    # On Windows (Git Bash), check for Visual Studio
    if [[ "${HOST_PLATFORM}" != "windows" ]]; then
        echo "ERROR: Windows builds require Windows host." >&2
        exit 1
    fi

    # Check if we're in a Developer Command Prompt or have VS installed
    if [[ -z "${VSINSTALLDIR}" ]] && [[ ! -d "/c/Program Files/Microsoft Visual Studio" ]] && [[ ! -d "/c/Program Files (x86)/Microsoft Visual Studio" ]]; then
        echo "WARNING: Visual Studio not detected. Build may fail." >&2
        echo "  Ensure Visual Studio 2022 is installed with C++ workload." >&2
    fi
}

# Display help
display_help() {
    cat << EOF
Ocean Unified Build Script (using CMake Presets)

Usage: $(basename "$0") [OPTIONS]

Target Platform:
  -p, --platform PLATFORM   Target platform: macos, linux, ios, android, windows
                            Default: current host platform (${HOST_PLATFORM})

Build Configuration:
  -c, --config CONFIG       Build configuration: debug, release, or both
                            (comma-separated). Default: debug,release
  -l, --link TYPE           Linking type: static, shared, or both
                            (comma-separated). Default: static
  -a, --arch ARCH           Architecture (comma-separated for multiple):
                              macos: arm64, x64
                              linux: x64, arm64
                              ios: arm64
                              android: arm64, arm32, x64
                              windows: x64, arm64
                            Default: auto-detect for native, arm64 for cross

Paths:
  -b, --build DIR           Build directory. Default: ./ocean_build
  -i, --install DIR         Install directory. Default: ./ocean_install
  -t, --third-party DIR     Third-party libraries directory
                            Default: ./ocean_install_thirdparty

Android-specific:
  -q, --quest               Build for Meta Quest (Android ARM64 with Quest extensions)
  --sdk VERSION             Android SDK version (e.g., android-32). Default: android-32

Build Options:
  -m, --minimal             Enable minimal build (base, cv, math only)
  -g, --generator GEN       CMake generator to use (overrides preset default)
                            Examples: "Ninja", "Unix Makefiles", "Xcode"
  --log-level LEVEL         CMake log level: ERROR, WARNING, NOTICE, STATUS,
                            VERBOSE, DEBUG, TRACE. Default: ERROR

Build Control:
  -s, --sequential          Build configurations sequentially (default: parallel)
  --configure-only          Only run CMake configure step, skip build

Information:
  --list-presets            List all available presets and exit
  -h, --help                Show this help message

Examples:
  # Build release static for current platform
  $(basename "$0")

  # Build for iOS from macOS
  $(basename "$0") -p ios

  # Build for Android with multiple ABIs
  $(basename "$0") -p android -a arm64,arm32,x64

  # Build for Quest
  $(basename "$0") --quest

  # Build with custom directories
  $(basename "$0") -b /path/to/build -i /path/to/install

  # Build with verbose CMake output
  $(basename "$0") --log-level STATUS

  # Minimal build
  $(basename "$0") -m

Direct preset usage (alternative):
  cmake --preset macos-arm64-static-release
  cmake --build --preset macos-arm64-static-release

EOF
}

# List available presets
list_presets() {
    echo "Available CMake presets:"
    echo ""
    echo "=== Configure Presets ==="
    cmake --list-presets=configure 2>/dev/null || true
    echo ""
    echo "=== Workflow Presets ==="
    cmake --list-presets=workflow 2>/dev/null || true
    echo ""
    echo "To use a preset directly:"
    echo "  cmake --preset <preset-name>"
    echo "  cmake --build --preset <preset-name>"
}

# Get default architecture for platform
get_default_arch() {
    local platform=$1

    case "${platform}" in
        macos|linux|windows)
            # Use host architecture for native builds
            echo "${HOST_ARCH}"
            ;;
        ios)
            echo "arm64"
            ;;
        android)
            echo "arm64"
            ;;
        *)
            echo "x64"
            ;;
    esac
}

# Validate architecture for platform
validate_arch() {
    local platform=$1
    local arch=$2

    case "${platform}" in
        macos)
            case "${arch}" in
                arm64|x64) return 0 ;;
                *) echo "ERROR: Invalid architecture '${arch}' for macOS. Use: arm64, x64" >&2; exit 1 ;;
            esac
            ;;
        linux)
            case "${arch}" in
                x64|arm64) return 0 ;;
                *) echo "ERROR: Invalid architecture '${arch}' for Linux. Use: x64, arm64" >&2; exit 1 ;;
            esac
            ;;
        ios)
            case "${arch}" in
                arm64) return 0 ;;
                *) echo "ERROR: Invalid architecture '${arch}' for iOS. Use: arm64" >&2; exit 1 ;;
            esac
            ;;
        android)
            case "${arch}" in
                arm64|arm32|x64) return 0 ;;
                *) echo "ERROR: Invalid architecture '${arch}' for Android. Use: arm64, arm32, x64" >&2; exit 1 ;;
            esac
            ;;
        windows)
            case "${arch}" in
                x64|arm64) return 0 ;;
                *) echo "ERROR: Invalid architecture '${arch}' for Windows. Use: x64, arm64" >&2; exit 1 ;;
            esac
            ;;
    esac
}

# Build a single configuration
run_build() {
    local platform=$1
    local arch=$2
    local link_type=$3
    local build_config=$4

    # Construct preset name
    local preset_name
    if [[ "${OCEAN_QUEST_MODE}" == "ON" && "${platform}" == "android" ]]; then
        preset_name="android-quest-${link_type}-${build_config}"
    else
        preset_name="${platform}-${arch}-${link_type}-${build_config}"
    fi

    if [[ "${OCEAN_LOG_LEVEL}" != "ERROR" ]]; then
        echo ""
        echo "========================================"
        echo "Building: ${preset_name}"
        echo "========================================"
        echo ""
    fi

    # Determine third-party path based on platform and config
    local third_party_platform="${platform}"
    local third_party_arch="${arch}"
    if [[ "${OCEAN_QUEST_MODE}" == "ON" && "${platform}" == "android" ]]; then
        third_party_arch="arm64"  # Quest is always ARM64
    fi

    local third_party_path="${OCEAN_THIRD_PARTY_DIR}/${third_party_platform}/${third_party_arch}_${link_type}_${build_config}"

    # Check if third-party directory exists
    if [[ ! -d "${third_party_path}" ]]; then
        echo "WARNING: Third-party directory not found: ${third_party_path}" >&2
        echo "         Build may fail if dependencies are not in system paths." >&2
    fi

    # Auto-detect subdivision structure
    local cmake_prefix_path=""
    if [[ -d "${third_party_path}/zlib" ]] || [[ -d "${third_party_path}/eigen" ]]; then
        # Per-library subdivision structure
        for libdir in "${third_party_path}"/*; do
            if [[ -d "$libdir" ]]; then
                if [[ -z "${cmake_prefix_path}" ]]; then
                    cmake_prefix_path="${libdir}"
                else
                    cmake_prefix_path="${cmake_prefix_path};${libdir}"
                fi
            fi
        done
    else
        # Flat structure
        cmake_prefix_path="${third_party_path}"
    fi

    # Determine build and install directories for this configuration
    local config_build_dir="${OCEAN_BUILD_DIR}/${platform}/${arch}_${link_type}_${build_config}"
    local config_install_dir="${OCEAN_INSTALL_DIR}/${platform}/${arch}_${link_type}_${build_config}"
    if [[ "${OCEAN_QUEST_MODE}" == "ON" && "${platform}" == "android" ]]; then
        config_build_dir="${OCEAN_BUILD_DIR}/${platform}/quest_${link_type}_${build_config}"
        config_install_dir="${OCEAN_INSTALL_DIR}/${platform}/quest_${link_type}_${build_config}"
    fi

    # Configure
    local configure_args=(
        --preset "${preset_name}"
        --log-level="${OCEAN_LOG_LEVEL}"
        -DCMAKE_PREFIX_PATH="${cmake_prefix_path}"
    )

    # Override build and install directories if custom paths specified
    configure_args+=(-B "${config_build_dir}")
    configure_args+=(-DCMAKE_INSTALL_PREFIX="${config_install_dir}")

    # Add generator override if specified
    if [[ -n "${OCEAN_GENERATOR}" ]]; then
        configure_args+=(-G "${OCEAN_GENERATOR}")
    fi

    # Add CMAKE_FIND_ROOT_PATH for cross-compilation (Android needs this)
    if [[ "${platform}" == "android" ]]; then
        configure_args+=(-DCMAKE_FIND_ROOT_PATH="${cmake_prefix_path}")
        configure_args+=(-DANDROID_PLATFORM="${OCEAN_ANDROID_SDK}")
    fi

    # Add minimal build flag if requested
    if [[ "${OCEAN_MINIMAL}" == "ON" ]]; then
        configure_args+=(-DOCEAN_BUILD_MINIMAL=ON)
        configure_args+=(-DOCEAN_BUILD_DEMOS=OFF)
        configure_args+=(-DOCEAN_BUILD_TESTS=OFF)
    fi

    cmake "${configure_args[@]}"

    if [[ "${OCEAN_INSTALL_TARGET}" == "OFF" ]]; then
        echo "Configure-only mode: skipping build step."
        return 0
    fi

    # Build and install
    # For multi-config generators (Xcode, VS), we need --config
    local build_args=(--build "${config_build_dir}" --target install)

    if [[ "${platform}" == "ios" ]]; then
        build_args+=(--config "${build_config^}")  # Capitalize first letter
        # Add Xcode-specific options
        if [[ "${OCEAN_LOG_LEVEL}" == "ERROR" ]]; then
            build_args+=(-- -quiet CODE_SIGNING_ALLOWED=NO -parallelizeTargets -jobs 16)
        else
            build_args+=(-- CODE_SIGNING_ALLOWED=NO -parallelizeTargets -jobs 16)
        fi
    elif [[ "${platform}" == "windows" ]]; then
        build_args+=(--config "${build_config^}")  # Capitalize first letter
        build_args+=(-- /m:16)
        if [[ "${OCEAN_LOG_LEVEL}" == "ERROR" ]]; then
            build_args+=(/v:q)
        fi
    else
        # Single-config generators (Ninja, Make)
        if [[ "${OCEAN_LOG_LEVEL}" == "ERROR" ]]; then
            build_args+=(-- -j16 -s)
        else
            build_args+=(-- -j16)
        fi
    fi

    cmake "${build_args[@]}"

    return $?
}

# Parse command-line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            display_help
            exit 0
            ;;
        --list-presets)
            cd "${OCEAN_SOURCE_DIR}"
            list_presets
            exit 0
            ;;
        -p|--platform)
            TARGET_PLATFORM="$2"
            shift 2
            ;;
        -c|--config)
            OCEAN_CONFIGS="$2"
            shift 2
            ;;
        -l|--link)
            OCEAN_LINK_TYPES="$2"
            shift 2
            ;;
        -a|--arch)
            OCEAN_ARCH="$2"
            shift 2
            ;;
        -b|--build)
            OCEAN_BUILD_DIR="$2"
            shift 2
            ;;
        -i|--install)
            OCEAN_INSTALL_DIR="$2"
            shift 2
            ;;
        -t|--third-party)
            OCEAN_THIRD_PARTY_DIR="$2"
            shift 2
            ;;
        -q|--quest)
            OCEAN_QUEST_MODE="ON"
            TARGET_PLATFORM="android"
            shift
            ;;
        --sdk)
            OCEAN_ANDROID_SDK="$2"
            shift 2
            ;;
        -m|--minimal)
            OCEAN_MINIMAL="ON"
            shift
            ;;
        -g|--generator)
            OCEAN_GENERATOR="$2"
            shift 2
            ;;
        --log-level)
            OCEAN_LOG_LEVEL="$2"
            shift 2
            ;;
        -s|--sequential)
            OCEAN_PARALLEL="OFF"
            shift
            ;;
        --configure-only)
            OCEAN_INSTALL_TARGET="OFF"
            shift
            ;;
        *)
            echo "ERROR: Unknown option: $1" >&2
            echo "Use --help for usage information." >&2
            exit 1
            ;;
    esac
done

# Check CMake version
check_cmake

# Set default target platform to host platform
if [[ -z "${TARGET_PLATFORM}" ]]; then
    TARGET_PLATFORM="${HOST_PLATFORM}"
fi

# Validate target platform
case "${TARGET_PLATFORM}" in
    macos|linux|ios|android|windows) ;;
    *)
        echo "ERROR: Unknown target platform: ${TARGET_PLATFORM}" >&2
        echo "  Supported platforms: macos, linux, ios, android, windows" >&2
        exit 1
        ;;
esac

# Validate cross-compilation is possible
validate_cross_compile "${TARGET_PLATFORM}"

# Platform-specific prerequisite checks
case "${TARGET_PLATFORM}" in
    android)
        check_android_prerequisites
        ;;
    ios)
        check_ios_prerequisites
        ;;
    windows)
        check_windows_prerequisites
        ;;
esac

# Set default architecture if not specified
if [[ -z "${OCEAN_ARCH}" ]]; then
    OCEAN_ARCH=$(get_default_arch "${TARGET_PLATFORM}")
fi

# Parse architectures (support comma-separated list)
IFS=',' read -ra ARCHS <<< "${OCEAN_ARCH}"
for arch in "${ARCHS[@]}"; do
    validate_arch "${TARGET_PLATFORM}" "${arch}"
done

# Validate log level
OCEAN_LOG_LEVEL=$(echo "${OCEAN_LOG_LEVEL}" | tr '[:lower:]' '[:upper:]')
if ! echo "${OCEAN_VALID_LOG_LEVELS}" | grep -qw "${OCEAN_LOG_LEVEL}"; then
    echo "ERROR: Invalid log level: ${OCEAN_LOG_LEVEL}" >&2
    echo "  Valid values: ${OCEAN_VALID_LOG_LEVELS}" >&2
    exit 1
fi

# Parse and validate configurations
IFS=',' read -ra CONFIGS <<< "${OCEAN_CONFIGS}"
for config in "${CONFIGS[@]}"; do
    case "${config}" in
        debug|release) ;;
        *)
            echo "ERROR: Invalid build configuration: ${config}" >&2
            echo "  Supported values: debug, release" >&2
            exit 1
            ;;
    esac
done

# Parse and validate link types
IFS=',' read -ra LINK_TYPES <<< "${OCEAN_LINK_TYPES}"
for link_type in "${LINK_TYPES[@]}"; do
    case "${link_type}" in
        static|shared) ;;
        *)
            echo "ERROR: Invalid linking type: ${link_type}" >&2
            echo "  Supported values: static, shared" >&2
            exit 1
            ;;
    esac
done

# Resolve directories to absolute paths
OCEAN_BUILD_DIR=$(mkdir -p "${OCEAN_BUILD_DIR}" && cd "${OCEAN_BUILD_DIR}" && pwd)
OCEAN_INSTALL_DIR=$(mkdir -p "${OCEAN_INSTALL_DIR}" && cd "${OCEAN_INSTALL_DIR}" && pwd)

if [[ -d "${OCEAN_THIRD_PARTY_DIR}" ]]; then
    OCEAN_THIRD_PARTY_DIR=$(cd "${OCEAN_THIRD_PARTY_DIR}" && pwd)
else
    echo "WARNING: Third-party directory not found: ${OCEAN_THIRD_PARTY_DIR}" >&2
    echo "         Proceeding anyway - build may fail if dependencies are missing." >&2
fi

# Change to source directory for preset resolution
cd "${OCEAN_SOURCE_DIR}"

# Display build plan (unless log level is ERROR for quiet mode)
if [[ "${OCEAN_LOG_LEVEL}" != "ERROR" ]]; then
    echo ""
    echo "Ocean Build Configuration"
    echo "========================="
    echo "  Host platform:   ${HOST_PLATFORM} (${HOST_ARCH})"
    echo "  Target platform: ${TARGET_PLATFORM}"
    echo "  Architecture(s): ${OCEAN_ARCH}"
    echo "  Configurations:  ${OCEAN_CONFIGS}"
    echo "  Link types:      ${OCEAN_LINK_TYPES}"
    echo "  Build dir:       ${OCEAN_BUILD_DIR}"
    echo "  Install dir:     ${OCEAN_INSTALL_DIR}"
    echo "  Third-party:     ${OCEAN_THIRD_PARTY_DIR}"
    echo "  Log level:       ${OCEAN_LOG_LEVEL}"
    if [[ -n "${OCEAN_GENERATOR}" ]]; then
        echo "  Generator:       ${OCEAN_GENERATOR}"
    else
        echo "  Generator:       (auto-detect)"
    fi
    if [[ "${OCEAN_QUEST_MODE}" == "ON" ]]; then
        echo "  Quest mode:      enabled"
    fi
    if [[ "${OCEAN_MINIMAL}" == "ON" ]]; then
        echo "  Minimal build:   enabled"
    fi
    if [[ "${TARGET_PLATFORM}" == "android" ]]; then
        echo "  Android SDK:     ${OCEAN_ANDROID_SDK}"
    fi
    echo "  Parallel:        ${OCEAN_PARALLEL}"
    echo ""
    echo "Builds to run:"
    for arch in "${ARCHS[@]}"; do
        for config in "${CONFIGS[@]}"; do
            for link_type in "${LINK_TYPES[@]}"; do
                if [[ "${OCEAN_QUEST_MODE}" == "ON" ]]; then
                    echo "  - android-quest-${link_type}-${config}"
                else
                    echo "  - ${TARGET_PLATFORM}-${arch}-${link_type}-${config}"
                fi
            done
        done
    done
    echo ""
fi

# Track failed builds
declare -a FAILED_BUILDS=()

# Run builds
if [[ "${OCEAN_PARALLEL}" == "ON" ]] && [[ ${#CONFIGS[@]} -gt 1 || ${#LINK_TYPES[@]} -gt 1 || ${#ARCHS[@]} -gt 1 ]]; then
    if [[ "${OCEAN_LOG_LEVEL}" != "ERROR" ]]; then
        echo "Building configurations in parallel..."
        echo ""
    fi

    FAILED_BUILDS_FILE=$(mktemp)
    trap "rm -f ${FAILED_BUILDS_FILE}" EXIT

    declare -a BUILD_PIDS=()

    for arch in "${ARCHS[@]}"; do
        for config in "${CONFIGS[@]}"; do
            for link_type in "${LINK_TYPES[@]}"; do
                (
                    if ! run_build "${TARGET_PLATFORM}" "${arch}" "${link_type}" "${config}"; then
                        if [[ "${OCEAN_QUEST_MODE}" == "ON" ]]; then
                            echo "android-quest-${link_type}-${config}" >> "${FAILED_BUILDS_FILE}"
                        else
                            echo "${TARGET_PLATFORM}-${arch}-${link_type}-${config}" >> "${FAILED_BUILDS_FILE}"
                        fi
                    fi
                ) &
                BUILD_PIDS+=($!)
            done
        done
    done

    # Wait for all builds
    for pid in "${BUILD_PIDS[@]}"; do
        wait "$pid" || true
    done

    # Collect failures
    if [[ -f "${FAILED_BUILDS_FILE}" ]]; then
        while IFS= read -r line; do
            FAILED_BUILDS+=("$line")
        done < "${FAILED_BUILDS_FILE}"
    fi
else
    if [[ "${OCEAN_LOG_LEVEL}" != "ERROR" ]]; then
        echo "Building configurations sequentially..."
        echo ""
    fi

    for arch in "${ARCHS[@]}"; do
        for config in "${CONFIGS[@]}"; do
            for link_type in "${LINK_TYPES[@]}"; do
                if ! run_build "${TARGET_PLATFORM}" "${arch}" "${link_type}" "${config}"; then
                    if [[ "${OCEAN_QUEST_MODE}" == "ON" ]]; then
                        FAILED_BUILDS+=("android-quest-${link_type}-${config}")
                    else
                        FAILED_BUILDS+=("${TARGET_PLATFORM}-${arch}-${link_type}-${config}")
                    fi
                fi
            done
        done
    done
fi

# Report results
if [[ ${#FAILED_BUILDS[@]} -eq 0 ]]; then
    if [[ "${OCEAN_LOG_LEVEL}" != "ERROR" ]]; then
        echo ""
        echo "========================================"
        echo "Build Summary"
        echo "========================================"
        echo "All builds completed successfully."
        echo ""
        echo "Install locations:"
        for arch in "${ARCHS[@]}"; do
            for config in "${CONFIGS[@]}"; do
                for link_type in "${LINK_TYPES[@]}"; do
                    if [[ "${OCEAN_QUEST_MODE}" == "ON" ]]; then
                        echo "  - ${OCEAN_INSTALL_DIR}/${TARGET_PLATFORM}/quest_${link_type}_${config}"
                    else
                        echo "  - ${OCEAN_INSTALL_DIR}/${TARGET_PLATFORM}/${arch}_${link_type}_${config}"
                    fi
                done
            done
        done
    fi
    exit 0
else
    echo "" >&2
    echo "========================================"  >&2
    echo "Build Summary" >&2
    echo "========================================" >&2
    echo "Some builds failed:" >&2
    for build in "${FAILED_BUILDS[@]}"; do
        echo "  - ${build}" >&2
    done
    exit 1
fi
