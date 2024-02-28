@REM (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

@echo off

cd vc143
call "build_win_vc143.bat" nopause
cd ..

if not "%1" == "nopause" (
pause
)
