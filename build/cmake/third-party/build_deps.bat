@REM Copyright (c) Meta Platforms, Inc. and affiliates.
@REM
@REM This source code is licensed under the MIT license found in the
@REM LICENSE file in the root directory of this source tree.

@echo off

setlocal EnableDelayedExpansion

set PLATFORM=%1
set OCEAN_THIRD_PARTY_SOURCE_DIR=%~2
set BUILD_DIRECTORY_BASE=%~3
set EXTRA_BUILD_FLAGS=%4
set SUBDIVIDE_INSTALL=%~5

shift
shift
shift
shift
shift

@REM Collect cmake arguments
set CMAKE_ARGS=
set BASE_INSTALL_PREFIX=
set BASE_FIND_ROOT_PATH=

:parse_args
if "%~1"=="" goto done_parsing
set arg=%~1
set CMAKE_ARGS=!CMAKE_ARGS! "%arg%"

@REM Extract base install prefix
echo !arg! | findstr /B /C:"-DCMAKE_INSTALL_PREFIX=" >nul
if !ERRORLEVEL! EQU 0 (
    set BASE_INSTALL_PREFIX=!arg:-DCMAKE_INSTALL_PREFIX=!
)

@REM Extract base find root path
echo !arg! | findstr /B /C:"-DCMAKE_FIND_ROOT_PATH=" >nul
if !ERRORLEVEL! EQU 0 (
    set BASE_FIND_ROOT_PATH=!arg:-DCMAKE_FIND_ROOT_PATH=!
)

shift
goto parse_args

:done_parsing

echo.
echo Building dependencies for platform: %PLATFORM%
echo Subdivision mode: %SUBDIVIDE_INSTALL%
if "%SUBDIVIDE_INSTALL%"=="ON" (
    echo Libraries will be installed to: %BASE_INSTALL_PREFIX%\[library_name]\
) else (
    echo Libraries will be installed to: %BASE_INSTALL_PREFIX%\
)
echo.

@REM Track previously built libraries for CMAKE_FIND_ROOT_PATH
set PREVIOUS_LIBS_PATH=%BASE_INSTALL_PREFIX%

@REM Process each dependency
for /F "eol=# usebackq delims=" %%d in ("%OCEAN_THIRD_PARTY_SOURCE_DIR%\dependencies_allplatforms.txt" "%OCEAN_THIRD_PARTY_SOURCE_DIR%\dependencies_%PLATFORM%.txt") do (
    echo.
    echo ========================================
    echo Building: %%d
    echo ========================================
    echo.

    if "%SUBDIVIDE_INSTALL%"=="ON" (
        @REM Per-library subdivision mode
        set LIB_INSTALL_PREFIX=%BASE_INSTALL_PREFIX%\%%d
        set LIB_FIND_ROOT_PATH=!PREVIOUS_LIBS_PATH!

        echo Installing to: !LIB_INSTALL_PREFIX!

        @REM Build cmake args with modified install prefix and find root path
        set LIB_CMAKE_ARGS=
        for %%a in (!CMAKE_ARGS!) do (
            set argval=%%~a
            echo !argval! | findstr /B /C:"-DCMAKE_INSTALL_PREFIX=" >nul
            if !ERRORLEVEL! EQU 0 (
                set LIB_CMAKE_ARGS=!LIB_CMAKE_ARGS! "-DCMAKE_INSTALL_PREFIX=!LIB_INSTALL_PREFIX!"
            ) else (
                echo !argval! | findstr /B /C:"-DCMAKE_FIND_ROOT_PATH=" >nul
                if !ERRORLEVEL! EQU 0 (
                    set LIB_CMAKE_ARGS=!LIB_CMAKE_ARGS! "-DCMAKE_FIND_ROOT_PATH=!LIB_FIND_ROOT_PATH!"
                ) else (
                    set LIB_CMAKE_ARGS=!LIB_CMAKE_ARGS! "!argval!"
                )
            )
        )

        cmake -S "%OCEAN_THIRD_PARTY_SOURCE_DIR%" -B "%BUILD_DIRECTORY_BASE%\%%d" -DINCLUDED_DEP_NAME=%%d !LIB_CMAKE_ARGS!
        if ERRORLEVEL 1 (
            exit /b 1
        )

        cmake --build "%BUILD_DIRECTORY_BASE%\%%d" --config %BUILD_TYPE% --target install -- %EXTRA_BUILD_FLAGS%
        if ERRORLEVEL 1 (
            exit /b 2
        )

        @REM Add this library to the path for future libraries
        if exist "!LIB_INSTALL_PREFIX!" (
            set PREVIOUS_LIBS_PATH=!PREVIOUS_LIBS_PATH!;!LIB_INSTALL_PREFIX!
        )
    ) else (
        @REM Flat structure mode (default)
        cmake -S "%OCEAN_THIRD_PARTY_SOURCE_DIR%" -B "%BUILD_DIRECTORY_BASE%\%%d" -DINCLUDED_DEP_NAME=%%d !CMAKE_ARGS!
        if ERRORLEVEL 1 (
            exit /b 1
        )

        cmake --build "%BUILD_DIRECTORY_BASE%\%%d" --config %BUILD_TYPE% --target install -- %EXTRA_BUILD_FLAGS%
        if ERRORLEVEL 1 (
            exit /b 2
        )
    )
)

echo.
echo All dependencies built successfully!
