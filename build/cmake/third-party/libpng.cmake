# (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.
message(CHECK_START "libpng")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

include(FetchContent)

set(ZLIB_ROOT ${CMAKE_INSTALL_PREFIX})

if(BUILD_SHARED_LIBS)
  set(PNG_SHARED ON CACHE BOOL "")
  set(PNG_STATIC OFF CACHE BOOL "")
else()
  set(PNG_SHARED OFF CACHE BOOL "")
  set(PNG_STATIC ON CACHE BOOL "")
endif()

set(PNG_TOOLS OFF CACHE BOOL "")

FetchContent_Declare(
  libpng
  GIT_REPOSITORY https://github.com/pnggroup/libpng.git
  GIT_TAG        ed217e3e601d8e462f7fd1e04bed43ac42212429 # 1.6.43
)
FetchContent_MakeAvailable(libpng)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
