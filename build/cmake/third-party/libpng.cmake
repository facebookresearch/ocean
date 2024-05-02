# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "libpng")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

find_package(Git REQUIRED)

set(ZLIB_ROOT ${CMAKE_INSTALL_PREFIX})

if(BUILD_SHARED_LIBS)
  set(PNG_SHARED ON CACHE BOOL "")
  set(PNG_STATIC OFF CACHE BOOL "")
else()
  set(PNG_SHARED OFF CACHE BOOL "")
  set(PNG_STATIC ON CACHE BOOL "")
endif()

set(PNG_TOOLS OFF CACHE BOOL "")
set(PNG_FRAMEWORK OFF)

CPMAddPackage(
  NAME           libpng
  GIT_REPOSITORY https://github.com/pnggroup/libpng.git
  GIT_TAG        ed217e3e601d8e462f7fd1e04bed43ac42212429 # 1.6.43
  PATCH_COMMAND  ${GIT_EXECUTABLE} apply "${CMAKE_CURRENT_SOURCE_DIR}/libpng/CMakeLists.txt.patch"
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
