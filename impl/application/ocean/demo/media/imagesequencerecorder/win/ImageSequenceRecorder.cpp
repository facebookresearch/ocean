/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/media/imagesequencerecorder/win/ImageSequenceRecorder.h"
#include "application/ocean/demo/media/imagesequencerecorder/win/ImageSequenceRecorderMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"

#include "ocean/io/Directory.h"

#include "ocean/platform/System.h"
#include "ocean/platform/Utilities.h"

#include "ocean/platform/win/System.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/directshow/DirectShow.h"
	#include "ocean/media/mediafoundation/MediaFoundation.h"
	#include "ocean/media/wic/WIC.h"
#endif

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nCmdShow*/)
{
#ifdef OCEAN_RUNTIME_STATIC
	Media::DirectShow::registerDirectShowLibrary();
	Media::MediaFoundation::registerMediaFoundationLibrary();
	Media::WIC::registerWICLibrary();
#else
	const IO::Directory pluginPath(Platform::Win::System::processDirectory() + IO::Directory("plugins"));

	if (pluginPath.exists())
	{
		PluginManager::get().collectPlugins(pluginPath());
	}

	if (PluginManager::get().unloadedPlugins().empty())
	{
		const std::string frameworkPath(Platform::Win::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));
		PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	}

	PluginManager::get().loadPlugins(PluginManager::TYPE_MEDIA);
#endif

	const Platform::Utilities::Commands commands(Platform::Utilities::parseCommandLine(lpCmdLine));

	std::string mediaFile;
	if (!commands.empty())
	{
		mediaFile = String::toAString(commands.front());
	}

	try
	{
		ImageSequenceRecorderMainWindow mainWindow(hInstance, L"Image recorder; 1:Change properties, 2: change format, a: start recorder, b: stop recorder", mediaFile);
		mainWindow.initialize();

		mainWindow.setFilename("output.jpg");
		mainWindow.start();
	}
	catch(...)
	{
		ocean_assert(false && "Unhandled exception!");
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::unregisterWICLibrary();
	Media::MediaFoundation::unregisterMediaFoundationLibrary();
	Media::DirectShow::unregisterDirectShowLibrary();
#else
	PluginManager::get().release();
#endif

	return 0;
}
