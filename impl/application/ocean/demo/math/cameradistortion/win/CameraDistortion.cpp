// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/math/cameradistortion/win/CameraDistortion.h"
#include "application/ocean/demo/math/cameradistortion/win/CameraDistortionMainWindow.h"

#include "ocean/base/Build.h"

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	try
	{
		CameraDistortionMainWindow mainWindow(hInstance, String::toWString(std::string("Camera Distortion (") + Build::buildString() + std::string(")")));
		mainWindow.initialize();
		mainWindow.start();
	}
	catch (...)
	{
		ocean_assert(false && "Unhandled exception");
	}

	return 0;
}
