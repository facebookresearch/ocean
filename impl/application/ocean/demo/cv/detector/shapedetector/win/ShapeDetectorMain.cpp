/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/shapedetector/win/ShapeDetectorMain.h"
#include "application/ocean/demo/cv/detector/shapedetector/win/ShapeDetectorMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/CommandArguments.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/PluginManager.h"

#include "ocean/platform/System.h"
#include "ocean/platform/Utilities.h"

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		#include "ocean/media/wic/WIC.h"
	#endif
#endif

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nCmdShow*/)
{
#ifdef OCEAN_RUNTIME_STATIC
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::registerWICLibrary());
#else
	const std::string frameworkPath(Platform::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::TYPE_MEDIA);
#endif

	CommandArguments commandArguments;
	commandArguments.registerNamelessParameters("Filenames of images which will be loaded.");
	commandArguments.registerParameter("help", "h", "Shows this help output");

	commandArguments.parse(lpCmdLine);

	if (commandArguments.hasValue("help"))
	{
		std::cout << commandArguments.makeSummary();
		return 0;
	}

	const std::vector<std::string> filenames = commandArguments.namelessValues();

	RandomI::initialize();

	try
	{
		ShapeDetectorMainWindow mainWindow(hInstance, String::toWString(std::string("Shape detector, ") + Build::buildString()), filenames);
		mainWindow.initialize();
		mainWindow.start();
	}
	catch(...)
	{
		ocean_assert(false && "Unhandled exception!");
	}

#ifdef OCEAN_RUNTIME_STATIC
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::unregisterWICLibrary());
#else
	PluginManager::get().release();
#endif

	return 0;
}
