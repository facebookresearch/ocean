@REM (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

@echo off

cd vc142
call "build_android_vc142.bat" nopause
cd ..

if not "%1" == "nopause" (
pause
)
