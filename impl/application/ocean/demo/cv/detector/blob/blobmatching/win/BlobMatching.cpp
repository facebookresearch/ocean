/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/blob/blobmatching/win/BlobMatching.h"
#include "application/ocean/demo/cv/detector/blob/blobmatching/win/BlobMatchingMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/String.h"

#include "ocean/io/Directory.h"

#include "ocean/platform/System.h"
#include "ocean/platform/Utilities.h"

#include "ocean/platform/win/System.h"

#include "ocean/system/Process.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/directshow/DSLibrary.h"
	#include "ocean/media/mediafoundation/MFLibrary.h"
	#include "ocean/media/wic/WIC.h"
#endif

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nCmdShow*/)
{
#ifdef OCEAN_RUNTIME_STATIC
	Media::MediaFoundation::MFLibrary::registerLibrary();
	Media::DirectShow::DSLibrary::registerLibrary();
	Media::WIC::registerWICLibrary();
#else
	const IO::Directory pluginPath(Platform::Win::System::processDirectory() + IO::Directory("plugins"));

	if (pluginPath.exists())
	{
		PluginManager::get().collectPlugins(pluginPath());
	}

	if (PluginManager::get().unloadedPlugins().empty())
	{
		const std::string frameworkPath(Platform::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));
		PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	}

	PluginManager::get().loadPlugins(PluginManager::TYPE_MEDIA);
#endif

	System::Process::setPriority(System::Process::PRIORITY_ABOVE_NORMAL);

	const Platform::Utilities::Commands commands(Platform::Utilities::parseCommandLine(lpCmdLine));

	std::string patternMedia;
	if (commands.size() >= 1)
	{
		patternMedia = String::toAString(commands[0]);
	}

	std::string inputMedia;
	if (commands.size() >= 2)
	{
		inputMedia = String::toAString(commands[1]);
	}

	std::string inputResolution;
	if (commands.size() >= 3)
	{
		inputResolution = String::toAString(commands[2]);
	}

	try
	{
		BlobMatchingMainWindow mainWindow(hInstance, String::toWString(std::string("Featuretracker Matching, ") + Build::buildString()).c_str(), patternMedia, inputMedia, inputResolution);
		mainWindow.initialize();
		mainWindow.start();
	}
	catch(...)
	{
		ocean_assert(false && "Unhandled exception!");
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::unregisterWICLibrary();
	Media::DirectShow::DSLibrary::unregisterLibrary();
	Media::MediaFoundation::MFLibrary::unregisterLibrary();
#else
	PluginManager::get().release();
#endif

	return 0;
}
