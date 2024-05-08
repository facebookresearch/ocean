/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/barcodes/detector2d/win/Application.h"
#include "application/ocean/demo/cv/detector/barcodes/detector2d/win/MainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/RandomI.h"

#include "ocean/platform/Utilities.h"

using namespace Ocean;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nCmdShow*/)
{
	Messenger::get().setOutputType(Messenger::OUTPUT_DEBUG_WINDOW);

	RandomI::initialize();

	try
	{
		MainWindow mainWindow(hInstance, std::wstring(L"oceandemocvdetectorbarcodesbarcodedetector2d, ") + String::toWString(Build::buildString()), Platform::Utilities::parseCommandLine(lpCmdLine));
	}
	catch(...)
	{
		ocean_assert(false && "Uncaught exception!");
	}

	return 0;
}
