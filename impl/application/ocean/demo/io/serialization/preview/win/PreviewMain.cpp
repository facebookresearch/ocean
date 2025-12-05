/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/io/serialization/preview/win/PreviewMain.h"
#include "application/ocean/demo/io/serialization/preview/win/PreviewMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/CommandArguments.h"
#include "ocean/base/PluginManager.h"

#include "ocean/platform/win/System.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/wic/WIC.h"
#endif

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::registerWICLibrary();
#else
	const std::string frameworkPath(Platform::Win::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::PluginType(PluginManager::TYPE_MEDIA));
#endif

	CommandArguments commandArguments;
	commandArguments.registerParameter("input", "i", "The input serialization file to preview");

	commandArguments.parse(__wargv, size_t(__argc));

	const std::wstring filename = String::toWString(commandArguments.value("input", std::string(), false, 0));

	try
	{
		PreviewMainWindow mainWindow(hInstance, std::wstring(L"Serialization Preview, ") + String::toWString(Build::buildString()), filename);
		mainWindow.initialize();
		mainWindow.start();
	}
	catch (...)
	{
		// nothing to do here
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::unregisterWICLibrary();
#else
	PluginManager::get().release();
#endif

	return 0;
}
