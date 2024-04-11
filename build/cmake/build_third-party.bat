@REM (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

@REM Determine the location of the source directory from the location of this script
set OCEAN_THIRD_PARTY_SOURCE_DIR=%~dp0..\..\build\cmake\third-party

@REM Static, debug + release
cmake -S %OCEAN_THIRD_PARTY_SOURCE_DIR% -B C:\tmp\ocean_third-party_build_static -DCMAKE_INSTALL_PREFIX=C:\tmp\ocean_third-party_install_static -DBUILD_PASS_INDEX=0

cmake --build C:\tmp\ocean_third-party_build_static --config Release --target install -- /m:16
cmake --build C:\tmp\ocean_third-party_build_static --config Debug --target install -- /m:16

cmake -S %OCEAN_THIRD_PARTY_SOURCE_DIR% -B C:\tmp\ocean_third-party_build_static -DCMAKE_INSTALL_PREFIX=C:\tmp\ocean_third-party_install_static -DBUILD_PASS_INDEX=1

cmake --build C:\tmp\ocean_third-party_build_static --config Release --target install -- /m:16
cmake --build C:\tmp\ocean_third-party_build_static --config Debug --target install -- /m:16

cmake -S %OCEAN_THIRD_PARTY_SOURCE_DIR% -B C:\tmp\ocean_third-party_build_static -DCMAKE_INSTALL_PREFIX=C:\tmp\ocean_third-party_install_static -DBUILD_PASS_INDEX=2

cmake --build C:\tmp\ocean_third-party_build_static --config Release --target install -- /m:16
cmake --build C:\tmp\ocean_third-party_build_static --config Debug --target install -- /m:16


@REM Shared, debug + release
cmake -S %OCEAN_THIRD_PARTY_SOURCE_DIR% -B C:\tmp\ocean_third-party_build_shared -DCMAKE_INSTALL_PREFIX=C:\tmp\ocean_third-party_install_shared -DBUILD_SHARED_LIBS=ON -DBUILD_PASS_INDEX=0

cmake --build C:\tmp\ocean_third-party_build_shared --config Release --target install -- /m:16
cmake --build C:\tmp\ocean_third-party_build_shared --config Debug --target install -- /m:16

cmake -S %OCEAN_THIRD_PARTY_SOURCE_DIR% -B C:\tmp\ocean_third-party_build_shared -DCMAKE_INSTALL_PREFIX=C:\tmp\ocean_third-party_install_shared -DBUILD_SHARED_LIBS=ON -DBUILD_PASS_INDEX=1

cmake --build C:\tmp\ocean_third-party_build_shared --config Release --target install -- /m:16
cmake --build C:\tmp\ocean_third-party_build_shared --config Debug --target install -- /m:16

cmake -S %OCEAN_THIRD_PARTY_SOURCE_DIR% -B C:\tmp\ocean_third-party_build_shared -DCMAKE_INSTALL_PREFIX=C:\tmp\ocean_third-party_install_shared -DBUILD_SHARED_LIBS=ON -DBUILD_PASS_INDEX=2

cmake --build C:\tmp\ocean_third-party_build_shared --config Release --target install -- /m:16
cmake --build C:\tmp\ocean_third-party_build_shared --config Debug --target install -- /m:16

