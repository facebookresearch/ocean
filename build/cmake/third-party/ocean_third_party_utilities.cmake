# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

# Helper function to write version.txt for a library (when using --subdivide)
# This helps the reorganization script know the library version
#
# Parameters:
#   VERSION_STRING: The version string to write (e.g., "1.3.1", "3.4.0")
#
# Example usage:
#   write_library_version("1.3.1")
#
function(write_library_version VERSION_STRING)
	get_filename_component(INSTALL_DIR_NAME "${CMAKE_INSTALL_PREFIX}" NAME)

	# List of known third-party library names (must match dependency names)
	set(KNOWN_LIBS
		"assimp" "curl" "directshow" "eigen" "freetype" "giflib" "gl" "googletest"
		"libjpeg-turbo" "libpng" "libtiff" "libusb" "libyuv"
		"mbedtls" "openxr" "protozero" "tinyxml2" "vtzero" "wxwidgets" "zlib"
		"android" "android_native_app_glue" "arcore-android-sdk"
	)

	# Only write version.txt when using --subdivide mode
	if("${INSTALL_DIR_NAME}" IN_LIST KNOWN_LIBS)
		# Write version.txt to install directory (with trailing newline)
		install(CODE "file(WRITE \"${CMAKE_INSTALL_PREFIX}/version.txt\" \"${VERSION_STRING}\n\")")
		message(STATUS "  Version: ${VERSION_STRING}")
	endif()
endfunction()

# Helper function to get the install root for a dependency library
# Handles both flat and per-library subdivision install structures
#
# When per-library subdivision is enabled:
#   - CMAKE_INSTALL_PREFIX = /path/to/install/current_lib
#   - Returns: /path/to/install/dependency_lib
#
# When flat structure is used:
#   - CMAKE_INSTALL_PREFIX = /path/to/install
#   - Returns: /path/to/install
#
# Parameters:
#   LIBRARY_NAME: Name of the dependency library (e.g., "zlib", "libjpeg-turbo")
#   OUTPUT_VAR: Variable to store the result
#
# Example usage:
#   include(${CMAKE_CURRENT_SOURCE_DIR}/ocean_third_party_utilities.cmake)
#   get_library_install_root("zlib" ZLIB_ROOT)
#   get_library_install_root("libjpeg-turbo" JPEG_ROOT)
#
function(get_library_install_root LIBRARY_NAME OUTPUT_VAR)
	get_filename_component(INSTALL_DIR_NAME "${CMAKE_INSTALL_PREFIX}" NAME)

	# List of known third-party library names (must match dependency names)
	set(KNOWN_LIBS
		"assimp" "curl" "directshow" "eigen" "freetype" "giflib" "gl" "googletest"
		"libjpeg-turbo" "libpng" "libtiff" "libusb" "libyuv"
		"mbedtls" "openxr" "protozero" "tinyxml2" "vtzero" "wxwidgets" "zlib"
		"android" "android_native_app_glue" "arcore-android-sdk"
	)

	if("${INSTALL_DIR_NAME}" IN_LIST KNOWN_LIBS)
		# Per-library subdivision is enabled
		# CMAKE_INSTALL_PREFIX ends with a library name, so get parent directory
		get_filename_component(BASE_PREFIX "${CMAKE_INSTALL_PREFIX}" DIRECTORY)
		set(${OUTPUT_VAR} "${BASE_PREFIX}/${LIBRARY_NAME}" PARENT_SCOPE)
	else()
		# Flat structure (default/legacy behavior)
		# CMAKE_INSTALL_PREFIX is the shared root for all libraries
		set(${OUTPUT_VAR} "${CMAKE_INSTALL_PREFIX}" PARENT_SCOPE)
	endif()
endfunction()
