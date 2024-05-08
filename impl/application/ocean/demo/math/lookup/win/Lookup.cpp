/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/math/lookup/win/Lookup.h"
#include "application/ocean/demo/math/lookup/win/LookupMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"

#include "ocean/platform/win/System.h"
#include "ocean/platform/win/Utilities.h"

using namespace Ocean;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	Messenger::get().setFileOutput("demomathlookup_output.txt");
	Messenger::get().setOutputType(Messenger::OUTPUT_FILE);

	try
	{
		LookupMainWindow mainWindow(hInstance, std::wstring(L"Demo Math Lookup ") + String::toWString(Build::buildString()));
		mainWindow.initialize();
		mainWindow.start();
	}
	catch(const Exception& e)
	{
		Log::error() << "Unhandled exception: " << e.what();
	}

	return 0;
}
