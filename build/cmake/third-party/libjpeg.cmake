# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "libjpeg")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

CPMAddPackage(
  NAME           libjpeg
  GIT_REPOSITORY https://github.com/daviddrysdale/libjpeg.git
  GIT_TAG        629681ab4f86387a6c04061a5f301dd8bb63d9c1
  PATCH_COMMAND  ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/libjpeg/CMakeLists.txt"
                                          "${CMAKE_CURRENT_SOURCE_DIR}/libjpeg/jconfig.h.cmake"
                                          .
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
