// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/tracking/homographyimagealigner/win/HIAMain.h"
#include "application/ocean/demo/tracking/homographyimagealigner/win/HIAMainWindow.h"

#include "ocean/base/Build.h"

#include "ocean/platform/Utilities.h"

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nCmdShow*/)
{
	try
	{
		HIAMainWindow mainWindow(hInstance, std::wstring(L"oceandemofeaturetracker, ") + String::toWString(Build::buildString()), Platform::Utilities::parseCommandLine(lpCmdLine));
	}
	catch(...)
	{
		ocean_assert(false && "Uncaught exception!");
	}

	return 0;
}
