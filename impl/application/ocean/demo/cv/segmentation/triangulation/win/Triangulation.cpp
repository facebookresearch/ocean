// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "application/ocean/demo/cv/segmentation/triangulation/win/TriangulationMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"

#include "ocean/platform/win/System.h"
#include "ocean/platform/win/Utilities.h"

using namespace Ocean;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	const std::string frameworkPath(Platform::Win::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

	try
	{
		TriangulationMainWindow mainWindow(hInstance, String::toWString(std::string("Triangulation ") + Build::buildString()));
		mainWindow.initialize();
		mainWindow.start();
	}
	catch(...)
	{
		ocean_assert(false && "Unhandled exception!");
	}

	return 0;
}
