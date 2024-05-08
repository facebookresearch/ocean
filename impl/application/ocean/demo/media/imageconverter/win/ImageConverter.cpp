/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/media/imageconverter/win/ImageConverter.h"
#include "application/ocean/demo/media/imageconverter/win/ImageConverterMainWindow.h"

#include "ocean/base/Build.h"
#include "ocean/base/Exception.h"
#include "ocean/base/PluginManager.h"

#include "ocean/platform/System.h"
#include "ocean/platform/Utilities.h"

#ifdef OCEAN_RUNTIME_STATIC
	#include "ocean/media/directshow/Directshow.h"
	#include "ocean/media/mediafoundation/MediaFoundation.h"
	#include "ocean/media/wic/WIC.h"
#endif

using namespace Ocean;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int /*nCmdShow*/)
{
	Messenger::get().setFileOutput("demomediaimageconverter_output.txt");
	Messenger::get().setOutputType(Messenger::OUTPUT_FILE);

#ifdef OCEAN_RUNTIME_STATIC
	Media::MediaFoundation::registerMediaFoundationLibrary();
	Media::DirectShow::registerDirectShowLibrary();
	Media::WIC::registerWICLibrary();
#else
	const std::string frameworkPath(Platform::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::TYPE_MEDIA);
#endif

	const Platform::Utilities::Commands commands(Platform::Utilities::parseCommandLine(lpCmdLine));

	std::string mediaFileInput;

	if (commands.size() > 0)
	{
		mediaFileInput = String::toAString(commands[0]);
	}

	try
	{
		ImageConverterMainWindow mainWindow(hInstance, L"Demo Media Image Converter", mediaFileInput);
		mainWindow.initialize();
		mainWindow.start();
	}
	catch(const Exception& e)
	{
		Log::error() << "Unhandled exception: " << e.what();
	}

#ifdef OCEAN_RUNTIME_STATIC
	Media::WIC::unregisterWICLibrary();
	Media::DirectShow::unregisterDirectShowLibrary();
	Media::MediaFoundation::unregisterMediaFoundationLibrary();
#else
	PluginManager::get().release();
#endif

	return 0;
}
