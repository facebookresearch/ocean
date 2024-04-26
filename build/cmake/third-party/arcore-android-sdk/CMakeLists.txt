cmake_minimum_required(VERSION 3.10)

message(CHECK_START "arcore-android-sdk")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(ARCORE_AAR_FILENAME "${CMAKE_CURRENT_BINARY_DIR}/arcode-android-sdk.aar")

file(DOWNLOAD
    https://dl.google.com/android/maven2/com/google/ar/core/1.30.0/core-1.30.0.aar
    ${ARCORE_AAR_FILENAME}
    EXPECTED_HASH SHA256=3863ce1368b7df4d4bd68846e5d5a6c457eefba0a73d034c3c27ed9a64854031
)

file(ARCHIVE_EXTRACT INPUT "${ARCORE_AAR_FILENAME}")

add_library(arcore SHARED IMPORTED)
set_target_properties(arcore PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/jni/${ANDROID_ABI}/libarcore_sdk_c.so
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include
)

install(IMPORTED_RUNTIME_ARTIFACTS arcore
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/arcore_c_api.h
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include
    COMPONENT include
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "completed")
