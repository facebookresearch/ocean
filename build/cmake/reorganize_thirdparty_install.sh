#!/usr/bin/env bash
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# Script to reorganize third-party library installations from CMake --subdivide
# structure to a version-first, platform-organized structure.
#
# Supported platforms: macOS, Linux, Android, iOS
#
# Input structure (--subdivide output):
#   install_3p/
#   ├── macos_static_Debug/
#   │   ├── zlib/
#   │   │   ├── include/
#   │   │   └── lib/
#   │   └── eigen/
#   │       └── include/
#   ├── android_arm64-v8a_static_Debug/
#   │   └── zlib/
#   │       ├── include/
#   │       └── lib/
#   └── ios_OS64_static_Debug/
#       └── zlib/
#           ├── include/
#           └── lib/
#
# Output structure (reorganized):
#   reorganized_3p/
#   ├── zlib/
#   │   └── 1.x.x/
#   │       ├── include/
#   │       └── lib/
#   │           ├── macos_static_debug/
#   │           ├── macos_static/
#   │           ├── android_arm64-v8a_static_debug/
#   │           └── ios_OS64_static_debug/
#   └── eigen/
#       └── 3.x.x/
#           └── include/

# This script can run on any Unix-like system (macOS, Linux, WSL)
# It will process all platform directories found in the input

echo "Reorganizing third-party libraries..."
echo ""

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

INPUT_DIR=""
OUTPUT_DIR="${PWD}/reorganized_3p"
VERSION_MAP_FILE="${SCRIPT_DIR}/third-party/library_versions.txt"

# Displays the supported parameters of this script
display_help()
{
    echo "Script to reorganize third-party library installations:"
    echo ""
    echo "Supported platforms: macOS, Linux, Android, iOS"
    echo ""
    echo "  $(basename "$0") [-h|--help] -i INPUT_DIR [-o OUTPUT_DIR] [-v VERSION_MAP]"
    echo ""
    echo "Arguments:"
    echo ""
    echo "  -i | --input INPUT_DIR : The directory containing --subdivide output"
    echo "                (e.g., install_3p with subdirectories like macos_static_Debug,"
    echo "                android_arm64-v8a_static_Debug, ios_OS64_static_Debug)"
    echo ""
    echo "  -o | --output OUTPUT_DIR : The output directory for reorganized structure."
    echo "                Default: ${OUTPUT_DIR}"
    echo ""
    echo "  -v | --version-map VERSION_MAP : Optional file mapping library names to versions."
    echo "                Default: ${VERSION_MAP_FILE}"
    echo ""
    echo "  -h | --help : This summary"
    echo ""
}

# Map CMake platform/config names to fbsource-style platform names
#
# Naming convention: {platform}_{arch}_{compiler}{version}_{linktype}[_debug]
#
# Examples:
#   - osx_arm64_xc15_static_debug (macOS ARM64, Xcode 15, static, debug)
#   - ios_arm64_xc15_static (iOS ARM64, Xcode 15, static)
#   - android_arm64_cl18_static (Android ARM64, Clang 18, static)
#   - win_x64_vc143_static (Windows x64, MSVC 14.3, static)
#   - linux_x64_gcc13_static_debug (Linux x64, GCC 13, static, debug)
#
# Input formats:
#   - 3 components: {platform}_{linktype}_{config} (macos, linux, windows)
#   - 4 components: {platform}_{variant}_{linktype}_{config} (android ABI, iOS toolchain)

# Detect compiler versions (cached for performance)
DETECTED_XCODE_VERSION=""
DETECTED_GCC_VERSION=""
DETECTED_CLANG_VERSION=""

get_xcode_version()
{
    if [[ -z "$DETECTED_XCODE_VERSION" ]]; then
        if command -v xcodebuild &> /dev/null; then
            DETECTED_XCODE_VERSION=$(xcodebuild -version 2>/dev/null | head -1 | awk '{print $2}' | cut -d. -f1)
        fi
        if [[ -z "$DETECTED_XCODE_VERSION" ]]; then
            DETECTED_XCODE_VERSION="15"  # Default fallback
        fi
    fi
    echo "$DETECTED_XCODE_VERSION"
}

get_gcc_version()
{
    if [[ -z "$DETECTED_GCC_VERSION" ]]; then
        if command -v gcc &> /dev/null; then
            DETECTED_GCC_VERSION=$(gcc -dumpversion 2>/dev/null | cut -d. -f1)
        fi
        if [[ -z "$DETECTED_GCC_VERSION" ]]; then
            DETECTED_GCC_VERSION="13"  # Default fallback
        fi
    fi
    echo "$DETECTED_GCC_VERSION"
}

get_android_clang_version()
{
    if [[ -z "$DETECTED_CLANG_VERSION" ]]; then
        # Try to get from NDK if available
        if [[ -n "$ANDROID_NDK" ]] && [[ -f "$ANDROID_NDK/toolchains/llvm/prebuilt/darwin-x86_64/bin/clang" ]]; then
            DETECTED_CLANG_VERSION=$("$ANDROID_NDK/toolchains/llvm/prebuilt/darwin-x86_64/bin/clang" --version 2>/dev/null | head -1 | grep -o 'clang version [0-9]*' | awk '{print $3}')
        elif [[ -n "$ANDROID_NDK" ]] && [[ -f "$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/clang" ]]; then
            DETECTED_CLANG_VERSION=$("$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/clang" --version 2>/dev/null | head -1 | grep -o 'clang version [0-9]*' | awk '{print $3}')
        fi
        if [[ -z "$DETECTED_CLANG_VERSION" ]]; then
            DETECTED_CLANG_VERSION="18"  # Default fallback (NDK r26+)
        fi
    fi
    echo "$DETECTED_CLANG_VERSION"
}

# Map Android ABI to architecture name
map_android_abi_to_arch()
{
    local abi="$1"
    case "$abi" in
        arm64-v8a)   echo "arm64" ;;
        armeabi-v7a) echo "arm" ;;
        x86_64)      echo "x64" ;;
        x86)         echo "x86" ;;
        *)           echo "$abi" ;;  # fallback
    esac
}

# Map iOS toolchain to architecture name
map_ios_toolchain_to_arch()
{
    local toolchain="$1"
    case "$toolchain" in
        OS64)           echo "arm64" ;;
        SIMULATOR64)    echo "x64" ;;
        SIMULATORARM64) echo "arm64" ;;
        OS)             echo "arm" ;;
        *)              echo "$toolchain" ;;  # fallback
    esac
}

map_platform_name()
{
    local cmake_name="$1"

    # Try 4-component match first (android, ios)
    # Format: {platform}_{variant}_{linktype}_{config}
    if [[ "$cmake_name" =~ ^([^_]+)_([^_]+)_([^_]+)_([^_]+)$ ]]; then
        local platform="${BASH_REMATCH[1]}"
        local variant="${BASH_REMATCH[2]}"
        local linktype="${BASH_REMATCH[3]}"
        local buildconfig="${BASH_REMATCH[4]}"

        local arch=""
        local compiler=""

        case "$platform" in
            android)
                arch=$(map_android_abi_to_arch "$variant")
                compiler="cl$(get_android_clang_version)"
                ;;
            ios)
                arch=$(map_ios_toolchain_to_arch "$variant")
                compiler="xc$(get_xcode_version)"
                ;;
            *)
                echo "ERROR: Unknown 4-component platform: $platform" >&2
                return 1
                ;;
        esac

        # Validate linking type
        case "$linktype" in
            static|shared) ;;
            *)
                echo "ERROR: Unknown linking type: $linktype" >&2
                return 1
                ;;
        esac

        # Build output name
        case "$buildconfig" in
            Debug)
                echo "${platform}_${arch}_${compiler}_${linktype}_debug"
                ;;
            Release)
                echo "${platform}_${arch}_${compiler}_${linktype}"
                ;;
            *)
                echo "ERROR: Unknown build config: $buildconfig" >&2
                return 1
                ;;
        esac
        return 0
    fi

    # Try 3-component match (macos, linux, windows)
    # Format: {platform}_{linktype}_{config}
    if [[ "$cmake_name" =~ ^([^_]+)_([^_]+)_([^_]+)$ ]]; then
        local platform="${BASH_REMATCH[1]}"
        local linktype="${BASH_REMATCH[2]}"
        local buildconfig="${BASH_REMATCH[3]}"

        local mapped_platform=""
        local arch=""
        local compiler=""

        case "$platform" in
            macos)
                mapped_platform="osx"
                if [[ $(uname -m) == "arm64" ]]; then
                    arch="arm64"
                else
                    arch="x64"
                fi
                compiler="xc$(get_xcode_version)"
                ;;
            linux)
                mapped_platform="linux"
                if [[ $(uname -m) == "aarch64" ]]; then
                    arch="arm64"
                else
                    arch="x64"
                fi
                compiler="gcc$(get_gcc_version)"
                ;;
            windows)
                mapped_platform="win"
                arch="x64"  # Default to x64, could be x86
                compiler="vc143"  # Default to VS2022
                ;;
            *)
                echo "ERROR: Unknown 3-component platform: $platform" >&2
                return 1
                ;;
        esac

        # Validate linking type
        case "$linktype" in
            static|shared) ;;
            *)
                echo "ERROR: Unknown linking type: $linktype" >&2
                return 1
                ;;
        esac

        # Build output name
        case "$buildconfig" in
            Debug)
                echo "${mapped_platform}_${arch}_${compiler}_${linktype}_debug"
                ;;
            Release)
                echo "${mapped_platform}_${arch}_${compiler}_${linktype}"
                ;;
            *)
                echo "ERROR: Unknown build config: $buildconfig" >&2
                return 1
                ;;
        esac
        return 0
    fi

    echo "ERROR: Invalid platform directory format: $cmake_name" >&2
    echo "       Expected: {platform}_{linktype}_{config} or {platform}_{variant}_{linktype}_{config}" >&2
    return 1
}

# Get library version from version.txt or fall back to header detection
get_library_version()
{
    local library_dir="$1"
    local library_name="$2"
    local library_include_dir="$3"

    # First priority: Check for version.txt in the library directory
    if [[ -f "${library_dir}/version.txt" ]]; then
        local version=$(cat "${library_dir}/version.txt" | tr -d '[:space:]')
        if [[ -n "$version" ]]; then
            echo "$version"
            return 0
        fi
    fi

    # Second priority: Try version map file if it exists
    if [[ -f "$VERSION_MAP_FILE" ]]; then
        local version=$(grep "^${library_name}=" "$VERSION_MAP_FILE" | cut -d'=' -f2)
        if [[ -n "$version" ]]; then
            echo "$version"
            return 0
        fi
    fi

    # Third priority: Try to infer version from common header patterns
    # This is library-specific and may need expansion
    case "$library_name" in
        zlib)
            if [[ -f "${library_include_dir}/zlib.h" ]]; then
                local version=$(grep "^#define ZLIB_VERSION" "${library_include_dir}/zlib.h" | awk '{print $3}' | tr -d '"')
                if [[ -n "$version" ]]; then
                    echo "$version"
                    return 0
                fi
            fi
            ;;
        eigen)
            if [[ -f "${library_include_dir}/Eigen/src/Core/util/Macros.h" ]]; then
                local major=$(grep "^#define EIGEN_WORLD_VERSION" "${library_include_dir}/Eigen/src/Core/util/Macros.h" | awk '{print $3}')
                local minor=$(grep "^#define EIGEN_MAJOR_VERSION" "${library_include_dir}/Eigen/src/Core/util/Macros.h" | awk '{print $3}')
                local patch=$(grep "^#define EIGEN_MINOR_VERSION" "${library_include_dir}/Eigen/src/Core/util/Macros.h" | awk '{print $3}')
                if [[ -n "$major" && -n "$minor" && -n "$patch" ]]; then
                    echo "${major}.${minor}.${patch}"
                    return 0
                fi
            fi
            ;;
        # Add more library-specific version detection here
    esac

    # Default fallback: use "current" as placeholder
    echo "current"
}

# Reorganize libraries from subdivide structure to version-first structure
reorganize_libraries()
{
    if [[ ! -d "$INPUT_DIR" ]]; then
        echo "ERROR: Input directory does not exist: $INPUT_DIR" >&2
        return 1
    fi

    echo "Input directory: $INPUT_DIR"
    echo "Output directory: $OUTPUT_DIR"
    echo ""

    # Create output directory
    mkdir -p "$OUTPUT_DIR"

    # Track processed libraries to consolidate headers
    # Note: Using space-separated strings instead of associative arrays for Bash 3.2 compatibility
    local library_versions_str=""
    local processed_headers_str=" "

    # Process each platform/config directory
    for platform_dir in "$INPUT_DIR"/*; do
        if [[ ! -d "$platform_dir" ]]; then
            continue
        fi

        local platform_basename=$(basename "$platform_dir")
        echo "Processing platform: $platform_basename"

        # Map to Ocean-native platform name
        local mapped_platform=$(map_platform_name "$platform_basename")
        if [[ $? -ne 0 ]]; then
            echo "WARNING: Skipping invalid platform directory: $platform_basename" >&2
            continue
        fi

        echo "  Mapped to: $mapped_platform"

        # Process each library in this platform directory
        for lib_dir in "$platform_dir"/*; do
            if [[ ! -d "$lib_dir" ]]; then
                continue
            fi

            local lib_name=$(basename "$lib_dir")
            echo "  Processing library: $lib_name"

            # Determine version
            local lib_include="${lib_dir}/include"
            local version=$(get_library_version "$lib_dir" "$lib_name" "$lib_include")

            echo "    Version: $version"

            # Create library/version structure
            local output_lib_dir="${OUTPUT_DIR}/${lib_name}/${version}"
            mkdir -p "$output_lib_dir"

            # Copy headers (only once per library - they should be identical across platforms)
            # Exclude cmake and pkgconfig directories and files
            if [[ ! "$processed_headers_str" =~ " $lib_name " ]] && [[ -d "$lib_include" ]]; then
                echo "    Copying headers (excluding cmake/pkgconfig files)..."
                mkdir -p "${output_lib_dir}/include"
                rsync -a --exclude='*.cmake' --exclude='cmake/' --exclude='pkgconfig/' "$lib_include"/ "${output_lib_dir}/include/"
                processed_headers_str="$processed_headers_str$lib_name "
            elif [[ -d "$lib_include" ]]; then
                echo "    Headers already copied (verifying consistency...)"
            else
                echo "    No include directory found at: $lib_include"
            fi

            # Copy libraries to platform-specific subdirectory
            # Exclude cmake and pkgconfig directories and files
            local lib_lib="${lib_dir}/lib"
            if [[ -d "$lib_lib" ]]; then
                echo "    Copying libraries to lib/${mapped_platform}/ (excluding cmake/pkgconfig files)..."
                local output_platform_lib="${output_lib_dir}/lib/${mapped_platform}"
                mkdir -p "$output_platform_lib"
                rsync -a --exclude='*.cmake' --exclude='*.pc' --exclude='cmake/' --exclude='pkgconfig/' "$lib_lib"/ "$output_platform_lib/"
            fi
        done

        echo ""
    done

    echo "Reorganization complete!"
    echo ""
    echo "Output structure:"
    tree -L 3 "$OUTPUT_DIR" 2>/dev/null || find "$OUTPUT_DIR" -type d -maxdepth 3
    echo ""
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    key="$1"
    case $key in
        -h|--help)
        display_help
        exit 0
        ;;
        -i|--input)
        INPUT_DIR="$2"
        shift
        shift
        ;;
        -o|--output)
        OUTPUT_DIR="$2"
        shift
        shift
        ;;
        -v|--version-map)
        VERSION_MAP_FILE="$2"
        shift
        shift
        ;;
        *)
        echo "ERROR: Unknown argument \"$1\"." >&2
        display_help
        exit 1
        ;;
    esac
done

# Validate required arguments
if [[ -z "$INPUT_DIR" ]]; then
    echo "ERROR: Input directory (-i) is required" >&2
    echo ""
    display_help
    exit 1
fi

# Resolve to absolute paths
INPUT_DIR=$( cd -- "$INPUT_DIR" &> /dev/null && pwd )
OUTPUT_DIR=$( cd -- "$( dirname -- "$OUTPUT_DIR" )" &> /dev/null && pwd )/$(basename "$OUTPUT_DIR")

# Run reorganization
reorganize_libraries

echo "Done!"
