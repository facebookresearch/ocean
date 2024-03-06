// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/cv/detector/fbmessengercodes/win/FBMessengerCodes.h"
#include "application/ocean/demo/cv/detector/fbmessengercodes/win/FBMessengerCodesMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/RandomI.h"

#include "ocean/platform/Utilities.h"

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nCmdShow*/)
{
	RandomI::initialize();

	try
	{
		FBMessengerCodesMainWindow mainWindow(hInstance, String::toWString(std::string("Messenger code detector, ") + Build::buildString()), Platform::Utilities::parseCommandLine(lpCmdLine));
	}
	catch(...)
	{
		ocean_assert(false && "Unhandled exception!");
	}

	return 0;
}
