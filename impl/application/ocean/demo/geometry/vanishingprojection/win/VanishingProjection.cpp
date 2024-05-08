/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/geometry/vanishingprojection/win/VanishingProjectionMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"

#include "ocean/platform/System.h"
#include "ocean/platform/Utilities.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/wic/WIC.h"
#endif

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nCmdShow*/)
{
	const std::string frameworkPath(Platform::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::registerWICLibrary();
#else
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::TYPE_MEDIA);
#endif

	const Platform::Utilities::Commands commands(Platform::Utilities::parseCommandLine(lpCmdLine));

	std::wstring mediaFile;
	if (!commands.empty())
	{
		mediaFile = commands.front();
	}

	try
	{
		VanishingProjectionMainWindow mainWindow(hInstance, String::toWString(std::string("Vanishing Projection ") + Build::buildString()), mediaFile);
		mainWindow.initialize();
		mainWindow.start();
	}
	catch(...)
	{
		ocean_assert(false && "Unhandled exception!");
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::unregisterWICLibrary();
#else
	PluginManager::get().release();
#endif

	return 0;
}
