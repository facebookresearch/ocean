file(GLOB_RECURSE NMAKE_CANDIDATES "C:/Program Files/Microsoft Visual Studio/nmake.exe")
foreach(CAND ${NMAKE_CANDIDATES})
  message(STATUS ${CAND})
  string(REGEX MATCH Hostx64/x64 MATCHOUT ${CAND})
  if(MATCHOUT)
    set(CMAKE_MAKE_PROGRAM ${CAND})
    break()
  endif()
endforeach()

#message(STATUS "found ${CMAKE_MAKE_PROGRAM}")
