/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/imageanalyzer/win/ImageAnalyzerMain.h"
#include "application/ocean/demo/cv/imageanalyzer/win/ImageAnalyzerMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/CommandArguments.h"
#include "ocean/base/PluginManager.h"

#include "ocean/platform/System.h"
#include "ocean/platform/Utilities.h"

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		#include "ocean/media/wic/WIC.h"
	#endif
#endif

using namespace Ocean;

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
	commandArguments.registerNamelessParameters("Optional the first parameter is interpreted as input parameter");
	commandArguments.registerParameter("input", "i", "The filename of the input image");
	commandArguments.registerParameter("help", "h", "Showing this help");

	commandArguments.parse(lpCmdLine);

	if (commandArguments.hasValue("help", nullptr, false))
	{
		std::cout << commandArguments.makeSummary() << std::endl;
		return 0;
	}

	std::vector<std::string> inputFilenames;

	const Value inputValue = commandArguments.value("input", false, 0);

	if (inputValue.isString())
	{
		inputFilenames.emplace_back(inputValue.stringValue());
	}
	else
	{
		inputFilenames = commandArguments.namelessValues();
	}

	try
	{
		ImageAnalyzerMainWindow mainWindow(hInstance, String::toWString(std::string("Image Analyzer (") + Build::buildString() + std::string(")")), inputFilenames);
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
