# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# Discovers the ARCore SDK header and shared library produced by
# build_ocean_3rdparty.py from the prebuilt AAR (`arcore-android-sdk`).
# The AAR's libarcore_sdk_c.so is installed under <prefix>/lib/ in the
# standard layout and under <root>/arcore-android-sdk/lib/<target>/ in the
# external-integration layout — both reachable via CMake's default search
# of CMAKE_PREFIX_PATH / CMAKE_LIBRARY_PATH set up by the top-level
# CMakeLists.txt.

find_path(ARCore_INCLUDE_DIR
    NAMES arcore_c_api.h
)

find_library(ARCore_LIBRARY
    NAMES arcore_sdk_c
)

if(ARCore_INCLUDE_DIR AND ARCore_LIBRARY)
    set(ARCore_FOUND TRUE)

    message(STATUS "ARCore: ${ARCore_LIBRARY}")

    add_library(ARCore::ARCore SHARED IMPORTED)
    set_target_properties(ARCore::ARCore PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${ARCore_INCLUDE_DIR}
        IMPORTED_LOCATION ${ARCore_LIBRARY}
    )
endif()
