# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

find_path(ARCore_INCLUDE_DIR
    NAMES arcore_c_api.h
    PATHS ${CMAKE_MODULE_PATH}/include
)

find_library(ARCore_LIBRARY
    NAMES libarcore_sdk_c.so
    PATHS ${CMAKE_MODULE_PATH}/lib
)

if(ARCore_INCLUDE_DIR AND ARCore_LIBRARY)
    set(ARCore_FOUND TRUE)

    message(STATUS "ARCore_FOUND")

    add_library(ARCore::ARCore SHARED IMPORTED)
    set_target_properties(ARCore::ARCore PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${ARCore_INCLUDE_DIR}
        IMPORTED_LOCATION ${ARCore_LIBRARY}
    )
endif()
