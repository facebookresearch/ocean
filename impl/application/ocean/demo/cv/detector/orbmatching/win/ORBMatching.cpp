/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/orbmatching/win/ORBMatching.h"
#include "application/ocean/demo/cv/detector/orbmatching/win/ORBMatchingMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/CommandArguments.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/String.h"

#include "ocean/io/Directory.h"

#include "ocean/platform/System.h"
#include "ocean/platform/Utilities.h"

#include "ocean/platform/win/System.h"

#include "ocean/system/Process.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/directshow/Directshow.h"
	#include "ocean/media/mediafoundation/MediaFoundation.h"
	#include "ocean/media/wic/WIC.h"
#endif

using namespace Ocean;

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
		const std::string frameworkPath(Platform::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));
		PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	}

	PluginManager::get().loadPlugins(PluginManager::TYPE_MEDIA);
#endif

	System::Process::setPriority(System::Process::PRIORITY_ABOVE_NORMAL);

	CommandArguments commandArguments("Demo application matching ORB features");
	commandArguments.registerNamelessParameters("Optional the first command argument is interpreted as pattern parameter");
	commandArguments.registerParameter("help", "h", "Showing this help output.");
	commandArguments.registerParameter("pattern", "p", "The file containing the pattern image for matching");
	commandArguments.registerParameter("input", "i", "The input to be used for matching, either an image, a video, or a live camera, 'LiveVideoId:1,0' by default");
	commandArguments.registerParameter("inputResolution", "ir", "Optional the preferred input resolution, either '320x240', '640x480', '1280x720', '1920x1080'", Value("1280x720"));

	Platform::Utilities::Commands commands = Platform::Utilities::parseCommandLine(lpCmdLine);

	commandArguments.parse(commands);

	if (commandArguments.hasValue("help"))
	{
		Log::info() << commandArguments.makeSummary();
		exit(0);
	}

	const std::string patternMedia = commandArguments.value<std::string>("pattern", "", false, 0);

	const std::string inputMedia = commandArguments.value<std::string>("input", "", false);

	const std::string inputResolution = commandArguments.value<std::string>("inputResolution", "", true);

	try
	{
		ORBMatchingMainWindow mainWindow(hInstance, String::toWString(std::string("ORB Feature Matching, ") + Build::buildString()).c_str(), patternMedia, inputMedia, inputResolution);

		mainWindow.initialize();
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
