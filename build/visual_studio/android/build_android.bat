@REM Copyright (c) Meta Platforms, Inc. and affiliates.
@REM
@REM This source code is licensed under the MIT license found in the
@REM LICENSE file in the root directory of this source tree.

@echo off

cd vc142
call "build_android_vc142.bat" nopause
cd ..

if not "%1" == "nopause" (
pause
)
