# adapted from https://sourceforge.net/p/giflib/feature-requests/_discuss/thread/cace39fc44/d77e/attachment/CMakeLists.txt

cmake_minimum_required(VERSION 3.26)

project(giflib C)

SET(BUILD_STATIC_LIBS OFF CACHE BOOL "Whether to also build static libs")

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



# Some utilities are installed
#set(giflib_INSTALLABLE
#    gif2rgb
#    gifbuild
#    gifecho
#    giffilter
#    giffix
#    gifinto
#    giftext
#    giftool
#    gifsponge
#    gifclrmp
#)

# Some utilities are only used internally for testing.
# There is a parallel list in doc/Makefile.
# These are all candidates for removal in future releases.
#set(giflib_UTILS
#    ${giflib_INSTALLABLE}
#    gifbg
#    gifcolor
#    gifhisto
#    gifwedge
#)

file(GLOB giflib_MAN doc/*.1)

### Build library / tools

add_library(gif SHARED ${giflib_SRC})

# On linux, at least, math library is separate in libm
# Windows includes libm in libc
find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(gif ${MATH_LIBRARY})
endif()

set_target_properties(gif PROPERTIES VERSION ${LIBVER} SOVERSION ${LIBMAJOR})
if(WIN32)
    set_target_properties(gif PROPERTIES SUFFIX "-${LIBMAJOR}${CMAKE_SHARED_LIBRARY_SUFFIX}")
    target_compile_definitions(gif PRIVATE "-Dstrtok_r=strtok_s")
endif(WIN32)

if(${BUILD_STATIC_LIBS})
    add_library(gif_static STATIC ${giflib_SRC})
    set_target_properties(gif_static PROPERTIES OUTPUT_NAME gif)
endif(${BUILD_STATIC_LIBS})

foreach(UTILITY ${giflib_UTILS})
    add_executable(${UTILITY} ${UTILITY}.c)
    target_link_libraries(${UTILITY} gif)
endforeach()

### Installation

install(TARGETS gif
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib${LIB_SUFFIX}
    LIBRARY DESTINATION lib${LIB_SUFFIX}
)

if(${BUILD_STATIC_LIBS})
    install(TARGETS gif_static ARCHIVE DESTINATION lib${LIB_SUFFIX})
endif(${BUILD_STATIC_LIBS})

foreach(UTILITY ${giflib_UTILS})
    install(TARGETS ${UTILITY} DESTINATION bin)
endforeach()

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
# We include all of the XML, and also generated manual pages
# so people working from the distribution tarball won't need xmlto.
add_custom_target(dist-gz
    COMMAND tar --transform='s:^:giflib-${VERSION}/:' -czf giflib-${VERSION}.tar.gz ${giflib_DIST}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(dist-bz2
    COMMAND tar --transform='s:^:giflib-${VERSION}/:' -cjf giflib-${VERSION}.tar.bz2 ${giflib_DIST}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(dist DEPENDS dist-gz DEPENDS dist-bz2)


### Auditing tools

# cppcheck should run clean
add_custom_target(cppcheck
    COMMAND cppcheck --inline-suppr --template gcc --enable=all --suppress=unusedFunction --force *.[ch]
)
