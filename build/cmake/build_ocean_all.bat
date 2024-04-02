@REM (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

@REM Determine the location of the source directory from the location of this script
set OCEAN_SOURCE_DIR=%~dp0..\..

@REM Static, debug + release
cmake -S %OCEAN_SOURCE_DIR% -B C:\tmp\ocean_build_static -DCMAKE_INSTALL_PREFIX=C:\tmp\ocean_install_static -DOCEAN_BUILD_THIRD_PARTY_LIBS=TRUE

cmake --build C:\tmp\ocean_build_static --config Release --target install -- /m:16
cmake --build C:\tmp\ocean_build_static --config Debug --target install -- /m:16

@REM Shared, debug + release
cmake -S %OCEAN_SOURCE_DIR% -B C:\tmp\ocean_build_shared -DCMAKE_INSTALL_PREFIX=C:\tmp\ocean_install_shared -DBUILD_SHARED_LIBS=ON -DOCEAN_BUILD_THIRD_PARTY_LIBS=TRUE

cmake --build C:\tmp\ocean_build_shared --config Release --target install -- /m:16
cmake --build C:\tmp\ocean_build_shared --config Debug --target install -- /m:16
