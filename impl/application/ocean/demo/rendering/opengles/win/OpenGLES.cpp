/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/rendering/opengles/win/OpenGLES.h"
#include "application/ocean/demo/rendering/opengles/win/OpenGLESMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/String.h"

using namespace Ocean;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	try
	{
		Messenger::get().setOutputType(Messenger::OUTPUT_DEBUG_WINDOW);

		OpenGLESMainWindow mainWindow(hInstance, String::toWString(std::string("OpenGLES (") + Build::buildString() + std::string(")")));
		mainWindow.initialize();
		mainWindow.start();
	}
	catch(...)
	{
		ocean_assert(false && "Unhandled exception!");
	}

	return 0;
}
