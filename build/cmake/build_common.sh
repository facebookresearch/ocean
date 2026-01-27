#!/usr/bin/env bash
# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# Common helper functions and dependency checks for Ocean build scripts.
# This script should be sourced by other build scripts, not executed directly.

# Detect the platform
detect_platform() {
  if [[ $(uname -s) == "Darwin" ]]; then
    echo "macos"
  elif [[ $(uname -s) == "Linux" ]]; then
    echo "linux"
  elif [[ $(uname -s) == *"MINGW"* ]]; then
    echo "windows"
  else
    echo "unknown"
  fi
}

# Check if a required command is available
# Arguments:
#   $1 - command name
#   $2 - optional: platform override (macos, linux, windows)
check_command() {
  local cmd=$1
  local platform=${2:-$(detect_platform)}

  if ! command -v "${cmd}" &> /dev/null; then
    echo "ERROR: ${cmd} is not installed or not in your PATH." >&2
    echo "" >&2
    echo "Please install ${cmd} before running this script:" >&2

    case "${cmd}" in
      cmake)
        case "${platform}" in
          macos)
            echo "  Using Homebrew:  brew install cmake" >&2
            echo "  Or download from: https://cmake.org/download/" >&2
            ;;
          linux)
            echo "  Ubuntu/Debian:   sudo apt-get install cmake" >&2
            echo "  Fedora/RHEL:     sudo dnf install cmake" >&2
            echo "  Or download from: https://cmake.org/download/" >&2
            ;;
          *)
            echo "  Download from: https://cmake.org/download/" >&2
            ;;
        esac
        ;;
      *)
        echo "  Please refer to the official documentation for installation instructions." >&2
        ;;
    esac

    return 1
  fi

  return 0
}

# Check all required build dependencies
# Arguments:
#   $1 - optional: platform override
check_build_dependencies() {
  local platform=${1:-$(detect_platform)}

  # OCEAN_DEVELOPMENT_PATH must be set
  if [ -z "${OCEAN_DEVELOPMENT_PATH}" ]; then
    echo "ERROR: OCEAN_DEVELOPMENT_PATH is not set." >&2
    echo "" >&2
    echo "Please set OCEAN_DEVELOPMENT_PATH to the root of your Ocean development directory:" >&2
    echo "  export OCEAN_DEVELOPMENT_PATH=/path/to/ocean" >&2
    exit 1
  fi

  # CMake is required for all builds
  check_command "cmake" "${platform}" || exit 1
}
