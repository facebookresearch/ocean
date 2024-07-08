/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/canvasfontcreator/win/CanvasFontCreatorMainWindow.h"

#include "ocean/base/Build.h"

using namespace Ocean;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	try
	{
		CanvasFontCreatorMainWindow mainWindow(hInstance, String::toWString(std::string("Canvas Font Creator (") + Build::buildString() + std::string(")")));
		mainWindow.initialize();
		mainWindow.start();
	}
	catch(...)
	{
		ocean_assert(false && "Unhandled exception!");
	}

	return 0;
}
