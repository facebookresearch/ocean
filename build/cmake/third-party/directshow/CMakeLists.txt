cmake_minimum_required(VERSION 3.15)

project(DirectShowBaseClasses
    LANGUAGES CXX
    DESCRIPTION "DirectShow Base Classes Library"
)

# Set C++ standard (the original code is compatible with older standards)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)

# Source files (from baseclasses.vcproj)
set(BASECLASSES_SOURCES
    amextra.cpp
    amfilter.cpp
    amvideo.cpp
    arithutil.cpp
    combase.cpp
    cprop.cpp
    ctlutil.cpp
    ddmm.cpp
    dllentry.cpp
    dllsetup.cpp
    mtype.cpp
    outputq.cpp
    perflog.cpp
    pstream.cpp
    pullpin.cpp
    refclock.cpp
    renbase.cpp
    schedule.cpp
    seekpt.cpp
    source.cpp
    strmctl.cpp
    sysclock.cpp
    transfrm.cpp
    transip.cpp
    videoctl.cpp
    vtrans.cpp
    winctrl.cpp
    winutil.cpp
    wxdebug.cpp
    wxlist.cpp
    wxutil.cpp
)

# Header files (for IDE organization)
set(BASECLASSES_HEADERS
    amextra.h
    amfilter.h
    cache.h
    checkbmi.h
    combase.h
    cprop.h
    ctlutil.h
    ddmm.h
    dllsetup.h
    dxmperf.h
    fourcc.h
    measure.h
    msgthrd.h
    mtype.h
    outputq.h
    perflog.h
    perfstruct.h
    pstream.h
    pullpin.h
    refclock.h
    reftime.h
    renbase.h
    schedule.h
    seekpt.h
    source.h
    streams.h
    strmctl.h
    sysclock.h
    transfrm.h
    transip.h
    videoctl.h
    vtrans.h
    winctrl.h
    winutil.h
    wxdebug.h
    wxlist.h
    wxutil.h
)

# Create the static library
add_library(strmbase STATIC
    ${BASECLASSES_SOURCES}
    ${BASECLASSES_HEADERS}
)

# Set output name based on configuration
# Debug builds get 'd' suffix, matching the original project
set_target_properties(strmbase PROPERTIES
    OUTPUT_NAME_DEBUG "strmbasd"
    OUTPUT_NAME_RELEASE "strmbase"
    OUTPUT_NAME_RELWITHDEBINFO "strmbase"
    OUTPUT_NAME_MINSIZEREL "strmbase"
)

# Include directories
target_include_directories(strmbase
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Preprocessor definitions
target_compile_definitions(strmbase
    PRIVATE
        WIN32
        _LIB
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
)

# Compiler options matching the original project
if(MSVC)
    target_compile_options(strmbase PRIVATE
        /W3  # Warning level 3
        $<$<CONFIG:Debug>:/Od>      # No optimization for debug
        $<$<CONFIG:Debug>:/RTC1>    # Runtime checks for debug
        $<$<CONFIG:Release>:/O2>    # Optimize for speed in release
    )

    # Use Unicode character set (matches default configuration)
    target_compile_definitions(strmbase PRIVATE
        UNICODE
        _UNICODE
    )

    # Set runtime library to match original project
    # /MD for Release (Multi-threaded DLL)
    # /MDd for Debug (Multi-threaded Debug DLL)
    set_target_properties(strmbase PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    )
endif()

# Link against required Windows libraries
target_link_libraries(strmbase
    PUBLIC
        strmiids  # DirectShow GUIDs library
        winmm     # Windows multimedia
)

# Optional: Create MBCS variants (Multi-Byte Character Set)
# Uncomment these sections if you need MBCS builds
#
# add_library(strmbase_mbcs STATIC
#     ${BASECLASSES_SOURCES}
#     ${BASECLASSES_HEADERS}
# )
#
# set_target_properties(strmbase_mbcs PROPERTIES
#     OUTPUT_NAME_DEBUG "strmbasd_mbcs"
#     OUTPUT_NAME_RELEASE "strmbase_mbcs"
# )
#
# target_include_directories(strmbase_mbcs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# target_compile_definitions(strmbase_mbcs PRIVATE WIN32 _LIB _MBCS
#     $<$<CONFIG:Debug>:_DEBUG>
#     $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
# )
# target_link_libraries(strmbase_mbcs PUBLIC strmiids winmm)

# Installation rules
install(TARGETS strmbase
    EXPORT DirectShowBaseClassesTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(FILES ${BASECLASSES_HEADERS}
    DESTINATION include
)

# Install CMake package configuration
include(CMakePackageConfigHelpers)

# Check if config template exists, otherwise create simple config
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/DirectShowBaseClassesConfig.cmake.in)
    # Generate the config file from template
    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/DirectShowBaseClassesConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/DirectShowBaseClassesConfig.cmake
        INSTALL_DESTINATION lib/cmake/DirectShowBaseClasses
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )
else()
    # Generate a basic config file inline
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/DirectShowBaseClassesConfig.cmake
"# DirectShow Base Classes CMake Configuration
include(\${CMAKE_CURRENT_LIST_DIR}/DirectShowBaseClassesTargets.cmake)
set(DirectShowBaseClasses_LIBRARIES DirectShow::strmbase)
")
endif()

# Generate version file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/DirectShowBaseClassesConfigVersion.cmake
    VERSION 1.0.0
    COMPATIBILITY SameMajorVersion
)

# Install the configuration files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/DirectShowBaseClassesConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/DirectShowBaseClassesConfigVersion.cmake
    DESTINATION lib/cmake/DirectShowBaseClasses
)

# Install the export set
install(EXPORT DirectShowBaseClassesTargets
    FILE DirectShowBaseClassesTargets.cmake
    NAMESPACE DirectShow::
    DESTINATION lib/cmake/DirectShowBaseClasses
)

# Print configuration information
message(STATUS "DirectShow Base Classes Configuration:")
message(STATUS "  Output (Debug):   strmbasd.lib")
message(STATUS "  Output (Release): strmbase.lib")
message(STATUS "  Character Set:    Unicode")
message(STATUS "  Install Prefix:   ${CMAKE_INSTALL_PREFIX}")
