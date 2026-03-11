@REM Copyright (c) Meta Platforms, Inc. and affiliates.
@REM
@REM This source code is licensed under the MIT license found in the
@REM LICENSE file in the root directory of this source tree.

@echo off

cd win
call "build_win.bat" nopause
cd ..

cd android
call "build_android.bat" nopause
cd ..

cd android_python
call "build_android_python.bat" nopause
cd ..

if not "%1" == "nopause" (
pause
)
