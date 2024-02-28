@REM (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
