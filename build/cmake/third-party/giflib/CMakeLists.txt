# adapted from https://sourceforge.net/p/giflib/feature-requests/_discuss/thread/cace39fc44/d77e/attachment/CMakeLists.txt

cmake_minimum_required(VERSION 3.26)

project(giflib C)

execute_process(COMMAND ./getversion
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(LIBMAJOR 5)
set(LIBMINOR 5)
set(LIBPOINT 2)
set(LIBVER "${LIBMAJOR}.${LIBMINOR}.${LIBPOINT}")

set(giflib_SRC
    dgif_lib.c
    egif_lib.c
    getarg.c
    gifalloc.c
    gif_err.c
    gif_font.c
    gif_hash.c
    openbsd-reallocarray.c
    qprintf.c
    quantize.c
)

file(GLOB giflib_MAN doc/*.1)

### Build library / tools

if(BUILD_SHARED_LIBS)
    add_library(gif SHARED ${giflib_SRC})
    #set_target_properties(gif PROPERTIES VERSION ${LIBVER} SOVERSION ${LIBMAJOR})
    if(WIN32)
        set_target_properties(gif PROPERTIES SUFFIX "-${LIBMAJOR}${CMAKE_SHARED_LIBRARY_SUFFIX}")
        target_compile_definitions(gif PRIVATE "-Dstrtok_r=strtok_s")
    endif(WIN32)
else()
    add_library(gif STATIC ${giflib_SRC})
endif()

# On linux, at least, math library is separate in libm
# Windows includes libm in libc
find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(gif ${MATH_LIBRARY})
endif()

### Installation

install(TARGETS gif
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
    LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)

install(FILES gif_lib.h DESTINATION include)
install(FILES ${giflib_MAN} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/man/man1)


### Distribution tarball
set(giflib_DIST
    *.c
    *.h
    README
    NEWS
    TODO
    COPYING
    getversion
    ChangeLog
    CMakeLists.txt
    build.adoc
    history.adoc
    control
    doc/whatsinagif
    doc/*.1
    doc/*.xml
    doc/*.txt
    doc/index.html.in
    doc/00README
    doc/Makefile
    tests
    pic
)

