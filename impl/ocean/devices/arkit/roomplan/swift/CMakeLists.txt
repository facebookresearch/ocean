# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.26)

# Disabled for now because the headers for Swift bridge interface are not generated correctly.

# if (IOS)

#     set(CMAKE_XCODE_ATTRIBUTE_SWIFT_OBJC_BRIDGING_HEADER "${CMAKE_CURRENT_LIST_DIR}/AKRoomPlanTracker6DOF_Objc.h")
#     set(CMAKE_XCODE_ATTRIBUTE_SWIFT_OBJC_INTERFACE_HEADER_NAME "${CMAKE_CURRENT_BINARY_DIR}/roomplan_swiftApple-Swift.h")

#     set(OCEAN_TARGET_NAME "ocean_devices_arkit_roomplan_swift")

#     find_library(ARKIT_LIB ARKit)
#     find_library(COREMOTION_LIB CoreMotion)
#     find_library(ROOMPLAN_LIB Roomplan)

#     # Source files
#     file(GLOB OCEAN_TARGET_HEADER_FILES "${CMAKE_CURRENT_LIST_DIR}/*.h")
#     file(GLOB OCEAN_TARGET_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/*.mm" "${CMAKE_CURRENT_LIST_DIR}/*.swift")

#     # Target definition
#     add_library(${OCEAN_TARGET_NAME} ${OCEAN_TARGET_SOURCE_FILES} ${OCEAN_TARGET_HEADER_FILES})

#     target_include_directories(${OCEAN_TARGET_NAME} PUBLIC "${OCEAN_IMPL_DIR}")

#     target_compile_definitions(${OCEAN_TARGET_NAME} PUBLIC ${OCEAN_PREPROCESSOR_FLAGS})

#     target_compile_options(${OCEAN_TARGET_NAME} PUBLIC "${OCEAN_COMPILER_FLAGS}")

#     # Dependencies
#     target_link_libraries(${OCEAN_TARGET_NAME}
#         PUBLIC
#             ocean_base
#             ocean_devices
#             ocean_devices_arkit_roomplan_native
#             ocean_math
#             ocean_media_avfoundation
#     )

#     target_link_libraries(${OCEAN_TARGET_NAME}
#         PUBLIC
#             ${ARKIT_LIB}
#             ${COREMOTION_LIB}
#             ${ROOMPLAN_LIB}
#     )

#     # Installation
#     install(TARGETS ${OCEAN_TARGET_NAME}
#             DESTINATION "${CMAKE_INSTALL_LIBDIR}"
#             COMPONENT lib
#     )

#     install(FILES ${OCEAN_TARGET_HEADER_FILES}
#             DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ocean/devices/arkit/roomplan/swift
#             COMPONENT include
#     )

# endif()
