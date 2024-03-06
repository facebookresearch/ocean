// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/tracking/oculustags/oculustagtracker/win/OculusTagTrackerMain.h"
#include "application/ocean/demo/tracking/oculustags/oculustagtracker/win/OculusTagTrackerMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/RandomI.h"

#include "ocean/platform/Utilities.h"

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nCmdShow*/)
{
	RandomI::initialize();

	try
	{
		OculusTagTrackerMainWindow mainWindow(hInstance, std::wstring(L"oceandemooculustagtracker, ") + String::toWString(Build::buildString()), Platform::Utilities::parseCommandLine(lpCmdLine));
	}
	catch(...)
	{
		ocean_assert(false && "Uncaught exception!");
	}

	return 0;
}
