@REM Copyright (c) Meta Platforms, Inc. and affiliates.
@REM
@REM This source code is licensed under the MIT license found in the
@REM LICENSE file in the root directory of this source tree.

@echo off

@echo Building the documentation for:
@echo.

@echo Ocean

del /Q ocean\ocean_doxygen.log
call python3 ocean\build.nopy
start cmd /C ocean\ocean_doxygen.log

@echo Application Ocean
del /Q application\ocean\application_ocean_doxygen.log
call python3 application\ocean\build.nopy
start cmd /C application\ocean\application_ocean_doxygen.log

@echo Ocean Interaction Java Script
del /Q interaction\javascript\interaction_javascript_doxygen.log
call python3 interaction\javascript\build.nopy
start cmd /C interaction\javascript\interaction_javascript_doxygen.log

pause
