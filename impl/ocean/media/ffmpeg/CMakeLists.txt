# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

if (MACOS OR ANDROID OR IOS OR LINUX OR WIN32)

    set(OCEAN_TARGET_NAME "ocean_media_ffmpeg")

    # Note: FFMPEG is currently not included in Ocean's third-party directory.
    # This file serves as a temporary placeholder, disabling FFMPEG usage in Ocean to prevent build failures.
    #
    # Once FFMPEG is integrated, search for and remove all instances of `OCEAN_DISABLE_FFMPEG_IN_CMAKE` to re-enable its functionality.

    add_library(
        ${OCEAN_TARGET_NAME}
        INTERFACE
    )

    set_target_properties(
        ${OCEAN_TARGET_NAME}
        PROPERTIES
            INTERFACE_COMPILE_DEFINITIONS "OCEAN_DISABLE_FFMPEG_IN_CMAKE"
    )

endif()
